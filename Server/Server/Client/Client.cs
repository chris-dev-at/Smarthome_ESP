using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Client
{
    /// <summary>
    /// Client used for outreach to ESP's
    /// </summary>
    public class Client
    {
        //public variables
        public bool Alive = true; //used for a keepAlive Loop which keeps the Application Alive
        public string serverIP { get; private set; }
        public int serverPort { get; private set; }
        public int buffer { get; set; }

        //private variables
        private Socket _clientsocket = new Socket(
            AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        
        //EventChains

        public class OnMessageEventArgs : DefaultEventArgs 
        {
            public string Message;
        }
        public class DefaultEventArgs : EventArgs
        {
            public string Reason;
        }



        //constructors
        public Client(string serverIP, int Port, int buffer)
        {
            this.serverIP = serverIP;
            this.serverPort = Port;
            this.buffer = buffer;

            StartUp();
        }

        #region ServerLogic

        public void StartUp()
        {
            
        }

        #endregion
    }
}
