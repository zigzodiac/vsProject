using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ATLCOMTest
{

    class Program
    {
        static void ClipInfoNotify(string in_sClipInfo)
        {
            Console.WriteLine("+++++++++++++++++++" + in_sClipInfo);
        }

        static void Main(string[] args)
        {
            
            ATLNetworkInterfaceComLib.NetworkComImpl com = new ATLNetworkInterfaceComLib.NetworkComImpl();
            com.SetHostInfo("192.168.1.119", 8055);
            com.SetCurrentTime("2020-11-12 16:35:20.120");
            com.Open();
            com.ClipInfoNotify += ClipInfoNotify;
            Console.ReadLine();
        }
    }
}
