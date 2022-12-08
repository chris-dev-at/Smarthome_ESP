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
        public int buffer { get; private set; }

        //private variables
        private Socket _clientsocket = new Socket(
            AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        private bool Alive = true; //used for a keepAlive Loop which keeps the Application Alive

        //EventChains
        public event EventHandler OnClientStart;
        public event EventHandler OnClientConnect;
        public event EventHandler OnClientMessage;
        public event EventHandler OnClientStop;



        //constructors
        public Client(string serverIP, int Port, int buffer)
        {
            this.serverIP = serverIP;
            serverPort = Port;
            this.buffer = buffer;

            OnClientStart += EventExecuted;
            OnClientConnect += EventExecuted;
            OnClientMessage += EventExecuted;
            OnClientStop += EventExecuted;
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
            Console.WriteLine($"Connected to {serverIP}:{serverPort}");

            //keep alive with requestloop
            Alive = true;
            new Thread(() =>
            {
                while (Alive)
                {
                    Thread.Sleep(100);
                    if (!_clientsocket.Connected)
                    {
                        ShutdownClient();
                        Environment.Exit(0);
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
                ShutdownClient("Lost Connection");
            }

            string text = ReadStreamAsText();
            if (text == null) return;

            string ReadStreamAsText()
            {
                try
                {
                    var buffer = new byte[this.buffer];
                    int received = _clientsocket.Receive(buffer, SocketFlags.None);
                    if (received == 0) return null;
                    var data = new byte[received];
                    Array.Copy(buffer, data, received);
                    string text = Encoding.ASCII.GetString(data);
                    return text;

                }
                catch (Exception)
                {
                    ShutdownClient("Server did not respond!");
                }
                return null;
            }
        }
        public void sendString(string msg)
        {
            byte[] buffer = Encoding.ASCII.GetBytes(msg);
            _clientsocket.Send(buffer, 0, buffer.Length, SocketFlags.None);
        }
        private void IncomingMsg(string msg)
        {
            OnClientMessage(this, new OnMessageEventArgs("Incomming Message", msg));
        }

        public void ShutdownClient(string Reason = "Client shutting down...")
        {
            OnClientStop(this, new DefaultClientEventArgs(Reason));
            Alive = false;
            this._clientsocket.Shutdown(SocketShutdown.Both);
            this._clientsocket.Close();
        }

        private void EventExecuted(object sender, EventArgs e) //is needed if nothing is contained in event chain
        {
            if (e is DefaultClientEventArgs)
                Console.WriteLine("Event: " + (e as DefaultClientEventArgs).Reason);
            if (e is OnMessageEventArgs)
                Console.WriteLine("Message: " + (e as OnMessageEventArgs).Reason + "\nContent: " + (e as OnMessageEventArgs).Message);
        }
        #endregion
    }
    public class OnMessageEventArgs : DefaultClientEventArgs
    {
        public string Message;
        public OnMessageEventArgs() { }
        public OnMessageEventArgs(string reason, string Message)
        {
            Reason = reason;
            this.Message = Message;
        }
    }
    public class DefaultClientEventArgs : EventArgs
    {
        public string Reason;
        public DefaultClientEventArgs() { }
        public DefaultClientEventArgs(string reason)
        {
            Reason = reason;
        }
    }
}
