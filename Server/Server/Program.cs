namespace Server
{
    internal class Program
    {
        static void Main(string[] args)
        {
            //test
            Client c1 = new Client("kolinsky.net", 25567, 256);
            c1.Connect();
            c1.sendString("0010204");
            while (true)
            {
                c1.sendString("20102000toggle11000010000");
                Thread.Sleep(3000);
                c1.sendString("20102000toggle01000010000");
                Thread.Sleep(3000);
            }
        }
    }
}