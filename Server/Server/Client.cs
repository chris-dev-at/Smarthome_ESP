using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Server
{
    /// <summary>
    /// Client used for outreach to ESP's
    /// </summary>
    public class Client
    {
        //public variables
        public string serverIP { get; private set; }
        public int serverPort { get; private set; }
        public int serverBuffer { get; private set; }
        public bool Alive { get; private set; } //used for a keepAlive Loop which keeps the Application Alive
        public bool logEvents { get; set; }

        //private variables
        private Socket _clientsocket = new Socket(
            AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        //EventChains
        public event EventHandler OnClientStart;
        public event EventHandler OnClientConnect;
        public event EventHandler OnClientMessage;
        public event EventHandler OnClientStop;



        //constructors
        public Client(string serverIP, int serverPort, int serverBuffer = 256, bool logEvents = false)
        {
            this.serverIP = serverIP;
            this.serverPort = serverPort;
            this.serverBuffer = serverBuffer;
            this.Alive = true;
            this.logEvents = logEvents;

            OnClientStart += EventLogger;
            OnClientConnect += EventLogger;
            OnClientMessage += EventLogger;
            OnClientStop += EventLogger;
        }

        public void Connect()
        {
            StartUp();
        }
        #region ClientLogic

        private void StartUp()
        {
            OnClientStart(this, new DefaultClientEventArgs
                ($"Client Connecting to {serverIP}:{serverPort}..."));
            while (!_clientsocket.Connected)
            {
                try
                {
                    _clientsocket.Connect(serverIP, serverPort);
                }
                catch { Console.WriteLine("Connection failed! retrying in 1000ms..."); Thread.Sleep(1000); }
            }
            OnClientConnect(this, new DefaultClientEventArgs($"Connected to {serverIP}:{serverPort}"));

            //keep alive with requestloop
            Alive = true;
            new Thread(() =>
            {
                while (Alive)
                {
                    Thread.Sleep(200);
                    if (!_clientsocket.Connected)
                    {
                        ShutdownClient("Client was Shutdown! {Alive=false}");
                    }
                }

            }).Start();

            new Thread(() =>
            {
                while (true)
                {
                    RequestLoop();
                }
            }).Start();
        }

        private void RequestLoop()
        {
            if (!_clientsocket.Connected)
            {
                ShutdownClient("Lost Connection... {RequestLoop finished}");
            }

            string text = ReadStream();
            if (text == null) return;

            OnClientMessage(this, new OnMessageClientEventArgs("Incomming Message", text));

            string ReadStream()
            {
                try
                {
                    var buffer = new byte[this.serverBuffer];
                    int received = _clientsocket.Receive(buffer, SocketFlags.None);
                    if (received == 0) return null;
                    var data = new byte[received];
                    Array.Copy(buffer, data, received);
                    string text = Encoding.ASCII.GetString(data);
                    return text;

                }
                catch (Exception)
                {
                    ShutdownClient("Server did not respond! {ReadStream}");
                }
                return null;
            }
        }
        public void sendString(string msg)
        {
            try
            {
                byte[] buffer = Encoding.ASCII.GetBytes(msg);
                _clientsocket.Send(buffer, 0, buffer.Length, SocketFlags.None);
            }
            catch (SocketException)
            {
            }
            catch (ObjectDisposedException)
            {
                ShutdownClient("Client was Disposed. Failed sending {sendString}");
            }
        }
        private void IncomingMsg(string msg)
        {
            OnClientMessage(this, new OnMessageClientEventArgs("Incomming Message", msg));
        }

        public void ShutdownClient(string Reason = "Client shutting down...")
        {
            if (Alive) //prevent Shutdown to happen on multiple occasions
            {
                OnClientStop(this, new DefaultClientEventArgs(Reason));
                Alive = false;
                this._clientsocket.Close();
            }
        }

        private void EventLogger(object sender, EventArgs e) //is needed if nothing is contained in event chain
        {
            if (this.logEvents)
            {
                if (e is DefaultClientEventArgs)
                    Console.WriteLine("Event: " + (e as DefaultClientEventArgs).Reason);
                if (e is OnMessageClientEventArgs)
                    Console.WriteLine("Message: " + (e as OnMessageClientEventArgs).Reason + "\nContent: " + (e as OnMessageClientEventArgs).Message);
                if (e is not DefaultClientEventArgs && e is not OnMessageClientEventArgs)
                    Console.WriteLine($"Unknown Event has been executed from {(sender as Client)}");
            }

        }
        #endregion

        #region Override

        public override string ToString() => $"Client: {this._clientsocket.RemoteEndPoint}";

        #endregion


    }
    public class OnMessageClientEventArgs : DefaultClientEventArgs
    {
        public string Message;
        public OnMessageClientEventArgs() { }
        public OnMessageClientEventArgs(string Reason, string Message)
        {
            this.Reason = Reason;
            this.Message = Message;
        }
    }
    public class DefaultClientEventArgs : EventArgs
    {
        public string Reason;
        public DefaultClientEventArgs() { }
        public DefaultClientEventArgs(string Reason)
        {
            this.Reason = Reason;
        }
    }
}
