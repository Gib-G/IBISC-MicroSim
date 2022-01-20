using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

namespace ViRTSA.Networking
{
    public class TCPClient : MonoBehaviour
    {
        private int PORT_NO = 29910;
        private string SERVER_IP = "10.0.0.1";
        private IPAddress serverIP;

        private TcpClient clientSocket = null;
        private NetworkStream stream = null;
        private bool socketException = false;

        private bool connected = false;
        public bool Connected => connected;

        private Thread clientThread = null;
        private EventWaitHandle _waitHandle = new AutoResetEvent(false);
        private bool running = false;

        private Queue<string> _queue;
        private readonly object _locker = new object();

        public delegate void DataCallback(string message);
        DataCallback answerCallback = null;

        public void Configure(DataCallback callback, string _ip = "10.0.0.1", int _port = 29910)
        {
            SERVER_IP = _ip;
            PORT_NO = _port;
            _queue = new Queue<string>();
            clientThread = new Thread(Work);

            // install the user defined data callback
            answerCallback = callback;

            clientSocket = new TcpClient();

            // get server host address
            serverIP = Dns.GetHostAddresses(SERVER_IP)[0];
            Debug.Log("TCPClient: server=" + SERVER_IP + "(" + serverIP + ") ; " + PORT_NO);
        }

        public void StartClient()
        {
            running = false;
            socketException = false;
            clientThread.Start();
            running = true;
        }

        void Work()
        {
            try
            {
                Debug.Log("TCPClient: trying to connect");
                clientSocket.Connect(new IPEndPoint(serverIP, PORT_NO));
                stream = clientSocket.GetStream();
                Debug.Log("TCPClient: connected");
                connected = true;

                while (running)
                {
                    _waitHandle.WaitOne();

                    string textToSend = null;
                    lock (_locker)
                        if (_queue.Count > 0)
                        {
                            textToSend = _queue.Dequeue();
                        }

                    SendToServer(textToSend);
                    string a = ReadFromServer();

                    answerCallback(a);
                }
            }
            catch (InvalidOperationException InvOpEx)
            {
                socketException = true;
                running = false;
                Debug.Log("TCPClient: TCP exception: " + InvOpEx.Message);
            }
            catch (SocketException SockEx)
            {
                socketException = true;
                running = false;
                Debug.Log("TCPClient: on Connect Socket exception: " + SockEx.Message);
            }
            catch (Exception ex)
            {
                socketException = true;
                running = false;
                Debug.Log("TCPClient: on Connect Socket exception: " + ex.Message);
            }
            finally
            {
                Debug.Log("TCPClient: Final polite closing");
                connected = false;
                running = false;

                if (stream != null)
                    stream.Close();
                stream = null;

                if (clientSocket != null)
                {
                    clientSocket.Close();
                }
                clientSocket = null;
            }
        }

        void SendToServer(string textToSend)
        {
            //---send the text---
            byte[] bytesToSend = ASCIIEncoding.ASCII.GetBytes(textToSend);
            //Debug.Log("Sending : " + textToSend);
            stream.Write(bytesToSend, 0, bytesToSend.Length);
        }

        string ReadFromServer()
        {
            //---read back the text---
            byte[] bytesToRead = new byte[clientSocket.ReceiveBufferSize];
            int bytesRead = stream.Read(bytesToRead, 0, clientSocket.ReceiveBufferSize);
            string answer = Encoding.ASCII.GetString(bytesToRead, 0, bytesRead);
            //Debug.Log("Received : " + answer);
            return answer;
        }

        public void EnqueueMessage(string aMessage)
        {
            lock (_locker) _queue.Enqueue(aMessage);
            // signal consumer that there is a new element
            _waitHandle.Set();
        }

        public void OnDisable()
        {
            stop();
            _waitHandle.Close();
        }

        public void stop()
        {
            Debug.Log("TCPClient: stop");
            connected = false;
            socketException = false;

            // ask the thread to stop
            running = false;

            // wait until the thread stops
            if (clientThread != null)
            {
                clientThread.Abort();
            }
            clientThread = null;

            if (stream != null)
                stream.Close();
            stream = null;

            if (clientSocket != null)
            {
                clientSocket.Close();
            }
            clientSocket = null;

            // Clean task queue
            if (_queue != null && _queue.Count > 0)
                _queue.Clear();
            _queue = null;
        }

        private void Update()
        {
            if (connected && !socketException)
            {
                SimulationManager.Instance.SetTCPServerUI(true);
            }
            else
            {
                SimulationManager.Instance.SetTCPServerUI(false);
            }
        }
    }
}