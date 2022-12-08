namespace Server
{
    internal class Program
    {
        static void Main(string[] args)
        {
            //test
            Client c1 = new Client("10.0.0.164", 25567, 256);
            c1.Connect();
            c1.sendString("0010116.");
            c1.sendString("20101000toggle1.");
            c1.OnClientMessage += newMessage;
            void newMessage(object sender, EventArgs e)
            {
                Console.WriteLine("Message:" + (e as OnMessageEventArgs).Message);
            }
        }
    }
}