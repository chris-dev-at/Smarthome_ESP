using System.Net.Sockets;
using System.Net;
using System.Security.Cryptography;

namespace Server
{
    internal class Program
    {
        static void Main(string[] args)
        {

            //DOES NOT WORK YET
            /*string IP = GetLocalIPAddress();
            Console.WriteLine(IP);
            string GetLocalIPAddress()
            {
                var host = Dns.GetHostEntry("TestHost123");
                foreach (var ip in host.AddressList)
                {
                    if (ip.AddressFamily == AddressFamily.InterNetwork)
                    {
                        return ip.ToString();
                    }
                }
                throw new Exception("No network adapters with an IPv4 address in the system!");
            }*/

            Client c1 = new Client("10.0.0.190", 25567, serverBuffer:1024, logEvents: true);
            c1.Connect();
            while (c1.Alive)
            {
                Console.WriteLine("App alive Client Alive: " + c1.Alive);
                c1.sendString("0010116.");  
                c1.sendString("20101000toggle1.");  
                c1.sendString("20101000toggle0.");  
                Thread.Sleep(1000);
            }
        }
    }
}