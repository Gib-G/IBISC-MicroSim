using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

namespace ViRTSA.Networking
{
    public class UDPListener : MonoBehaviour
    {
        private int PORT_NO = 29900;
        private string LISTENER_IP = "10.0.0.2";
        private int UDP_PACKAGE_SIZE = 255;
        private IPEndPoint groupEP = null;

        private UdpClient listener = null;
        private bool socketException = false;
        private bool connected = false;

        private Thread listenerThread = null;
        private bool running = false;

        public delegate void DataCallback(string message);
        DataCallback subscriberCallback = null;

        public void Configure(DataCallback callback, string _ip = "10.0.0.2", int _port = 29900)
        {
            LISTENER_IP = _ip;
            PORT_NO = _port;
            listenerThread = new Thread(Work);

            // install the user defined data callback
            subscriberCallback = callback;

            listener = new UdpClient(PORT_NO);
            listener.Client.ReceiveBufferSize = UDP_PACKAGE_SIZE;

            IPAddress ip = IPAddress.Any;
            groupEP = new IPEndPoint(Dns.GetHostAddresses(LISTENER_IP)[0], PORT_NO);
            //groupEP = new IPEndPoint(ip, PORT_NO);
            Debug.Log("UDPListener: listening at=" + LISTENER_IP + " (" + PORT_NO + ")");
        }

        public void StartListener()
        {
            running = true;
            listenerThread.Start();
            connected = true;
        }

        void Work()
        {
            try
            {
                while (running)
                {
                    byte[] bytes = listener.Receive(ref groupEP);
                    string a = Encoding.ASCII.GetString(bytes, 0, bytes.Length);
                    //Debug.Log("NEW MSG = " + a + "<END>");
                    subscriberCallback(a);
                }

            }
            catch (InvalidOperationException InvOpEx)
            {
                socketException = true;
                running = false;
                Debug.Log("UDPListener: TCP exception: " + InvOpEx.Message);
            }
            catch (SocketException SockEx)
            {
                socketException = true;
                running = false;
                Debug.Log("UDPListener: on Connect Socket exception: " + SockEx.Message);
            }
            catch (Exception ex)
            {
                socketException = true;
                running = false;
                Debug.Log("UDPListener: on Connect Socket exception: " + ex.Message);
            }
            finally
            {
                if (listener != null)
                {
                    listener.Close();
                }
                listener = null;
            }
        }

        public void OnDisable()
        {
            stop();
        }

        public void stop()
        {
            Debug.Log("UDPClient: stop");

            // ask the thread to stop
            running = false;

            // wait until the thread stops
            if (listenerThread != null)
            {
                listenerThread.Abort();
            }
            listenerThread = null;

            if (listener != null)
            {
                listener.Close();
            }
            listener = null;
        }
    }
}