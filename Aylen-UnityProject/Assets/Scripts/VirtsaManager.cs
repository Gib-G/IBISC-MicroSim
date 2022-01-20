using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Threading;
using UnityEngine;
//using ViRTSA.Log;
using ViRTSA.Networking;
//using ViRTSA.User;

namespace ViRTSA.Plugin
{
    public enum ViRTSA_FUNCTION_CODE
    {
        PING = -1,
        INITIALIZE_VIRTSA = 1,  // 5 parameters ({true;false},{true;false},{true;false},COM_PORT,BAUD_RATE)
        SET_VIRTUAL_SCENE = 2,
        START_VIRTSA = 3,
        FINISH_VIRTSA = 4,
        UPDATE_GAME_SCENE_POSITION = 5, // 3 parameters (x,y,z)
        SET_HAPTIC_FORCES = 6,  // 1 parameter ({true;false})
        RESET_OBJECTS_POSITIONS = 7,
        GET_NUMBER_OF_TOOLS = 8,
        GET_INTERACT_OBJECTS = 9,
        GET_HAPTIC_INFORMATION = 10,
        GET_RIGID_OBJECTS = 11,
        UPDATE_DYNAMICS = 12,
        RESET_OBJECT_POSITION = 13,
        UPDATE_TOOLS = -8,
        WAITING_FOR_INSTRUCTION = 0
    }

    public class VirtsaManager : MonoBehaviour
    {
        private TCPClient TCP_client;
        private UDPListener UDP_listener;

        private EvaluationConfig Configuration { get { return SimulationManager.Instance.Configuration; } }
        //private UserInformation CurrentUser { get { return SimulationManager.Instance.CurrentUser; } }

        private ViRTSA_FUNCTION_CODE state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
        private const string SEPARATOR = " ";

        public List<CubeManager> cubesManagers;
        public List<ForcepsManager> devicesManagers;
        private double[,] cubesMat = new double[6, 7];

        private readonly object udpDataLock = new object();
        private string Data = "";

        private string numberOfDevStr = "0";
        private int numberOfDev = 0;

        private bool tcp_enabled { get { return TCP_client != null && TCP_client.Connected; } }
        private string TCP_message = "";
        private bool waitingForAnswer = false;

        [SerializeField]
        private bool performPing = true;
        private bool ping = false;

        private void Start()
        {
            StartCoroutine(EnqueueTCPMessage());
            StartCoroutine(Ping());
        }

        private void OnDisable()
        {
            StopCoroutine(EnqueueTCPMessage());
            StopCoroutine(Ping());
        }

        private void Update()
        {
            UpdateTransforms();
        }

        public void TCPDataCallback(string data)
        {
            Debug.Log("TCP message answer: " + data);
            string[] retArray;

            switch (state)
            {
                case ViRTSA_FUNCTION_CODE.INITIALIZE_VIRTSA:
                    retArray = data.Split(new char[0]);
                    numberOfDevStr = retArray[1];
                    state = ViRTSA_FUNCTION_CODE.UPDATE_TOOLS;
                    break;
                case ViRTSA_FUNCTION_CODE.FINISH_VIRTSA:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.UPDATE_GAME_SCENE_POSITION:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.SET_HAPTIC_FORCES:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.RESET_OBJECTS_POSITIONS:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.RESET_OBJECT_POSITION:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.GET_NUMBER_OF_TOOLS:
                    retArray = data.Split(new char[0]);
                    numberOfDevStr = retArray[1];
                    state = ViRTSA_FUNCTION_CODE.UPDATE_TOOLS;
                    break;
                case ViRTSA_FUNCTION_CODE.GET_INTERACT_OBJECTS:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.GET_HAPTIC_INFORMATION:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.GET_RIGID_OBJECTS:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                case ViRTSA_FUNCTION_CODE.UPDATE_DYNAMICS:
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
                default:
                    //state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    break;
            }
            waitingForAnswer = false;
        }

        public void ReconnectTCPServer()
        {
            TCP_client.stop();
            TCP_client.Configure(TCPDataCallback, Configuration.TCPserverAddr, Configuration.TCPport);
            TCP_client.StartClient();
        }

        public void DisconnectTCPServer()
        {
            TCP_client.stop();
        }

        public void UDPDataCallback(string data)
        {
            Monitor.Enter(udpDataLock);
            Data = new string(data.ToCharArray());
            Monitor.Exit(udpDataLock);
        }

        public void UpdateTransforms()
        {
            string data;

            Monitor.Enter(udpDataLock);
            data = new string(Data.ToCharArray());
            Monitor.Exit(udpDataLock);

            // LOG 
            /*if (SimulationManager.Instance.logData && data != "")
            {
                string time = "";
                Monitor.Enter(SimulationManager.Instance._locker);
                time = SimulationManager.Instance.TaskTime;
                Monitor.Exit(SimulationManager.Instance._locker);

                SimulationManager.Instance.PositioningData_ProducerConsumer.EnqueueTask(new PositioningData(CurrentUser.Identifier + "," + time + "," + data));
            }*/

            string[] retArray = data.Split(new char[0]);
            if (retArray.Length <= 2) return;

            double val = 0;

            int temp = 0;

            int offset = 2;
            if (retArray.Length >= 44)
            {
                // get 6 cubes pos and rot
                for (int i = 0; i < 6; i++)
                {
                    double[] valMat = new double[7];
                    for (int j = 0; j < 7; j++)
                    {
                        if (!Double.TryParse(retArray[offset + 7 * i + j], NumberStyles.Any, CultureInfo.InvariantCulture, out val))
                            val = 0;
                        valMat[j] = val;
                        temp++;
                    }
                    Vector3 pos = new Vector3(-(float)valMat[0], -(float)valMat[1], -(float)valMat[2]);
                    Quaternion rot = new Quaternion((float)valMat[3], (float)valMat[4], (float)valMat[5], (float)valMat[6]);

                    cubesManagers[i].UpdatePosition(pos, rot);
                }
            }

            offset = 44;
            if (retArray.Length >= 58)
            {
                int qtyDev = (retArray.Length - 44) / 14;
                for (int i = 0; i < qtyDev; i++)    // for two devices
                {
                    Quaternion rot;

                    double[] rotMat = new double[4];
                    for (int j = 0; j < 4; j++)
                    {
                        if (!Double.TryParse(retArray[offset + 20 * i + j], NumberStyles.Any, CultureInfo.InvariantCulture, out val))
                            val = 0;
                        rotMat[j] = val;
                        temp++;
                    }
                    rot = new Quaternion((float)rotMat[0], (float)rotMat[1], (float)rotMat[2], (float)rotMat[3]);
                    ForcepsInfo forcep = new ForcepsInfo();
                    forcep.CurrentRotation = rot;
                    List<Fingerinfo> fingers = new List<Fingerinfo>();

                    for (int k = 0; k < 2; k++) // for each: finger and thumb
                    {
                        double[] valMat = new double[8];
                        int offset2 = offset + 20 * i + 4;
                        for (int j = 0; j < 8; j++)
                        {
                            if (!Double.TryParse(retArray[offset2 + 8 * k + j], NumberStyles.Any, CultureInfo.InvariantCulture, out val))
                                val = 0;
                            valMat[j] = val;
                            temp++;
                        }
                        //Vector3 metaPos = new Vector3(-(float)valMat[0], -(float)valMat[1], -(float)valMat[2]);
                        Vector3 hapticPos = new Vector3(-(float)valMat[3], -(float)valMat[4], -(float)valMat[5]);
                        float forcepsIdealAngle = (float)valMat[6]; // NOT USED (from my previous app)
                        float forcepsCalculatedAngle = (float)valMat[7];    // NOT USED (from my previous app)
                        Fingerinfo finger = new Fingerinfo(hapticPos, forcepsIdealAngle, forcepsCalculatedAngle);
                        fingers.Add(finger);
                    }
                    forcep.Finger = fingers[0];
                    forcep.Thumb = fingers[1];
                    fingers.Clear();

                    devicesManagers[i].UpdatePositions(forcep);
                }
            }
        }

        public void SetTCPClient(TCPClient tcp_client)
        {
            TCP_client = tcp_client;
        }

        public void SetUDPClient(UDPListener udp_listener)
        {
            UDP_listener = udp_listener;
        }

        public void NewSimulation()
        {
            TCP_client.Configure(TCPDataCallback, Configuration.TCPserverAddr, Configuration.TCPport);
            UDP_listener.Configure(UDPDataCallback, Configuration.UDPlisteningAddr, Configuration.UDPport);

            TCP_client.StartClient();
            UDP_listener.StartListener();
        }

        public void StartVirtsaChai3d()
        {
            // send first command message: Setup chai3d ODE ViRTSA game scene
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.INITIALIZE_VIRTSA).ToString() + SEPARATOR + "5" + SEPARATOR +
                "true true false" + SEPARATOR + Configuration.COMport.ToString() +
                SEPARATOR + Configuration.BaudRateSerial.ToString();
            state = ViRTSA_FUNCTION_CODE.INITIALIZE_VIRTSA;
        }

        public void StopApplication()
        {
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.FINISH_VIRTSA).ToString() + SEPARATOR + "0";
            state = ViRTSA_FUNCTION_CODE.FINISH_VIRTSA;
        }

        public void GetHapticDevicesCount()
        {
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.GET_NUMBER_OF_TOOLS).ToString() + SEPARATOR + "0";
            state = ViRTSA_FUNCTION_CODE.GET_NUMBER_OF_TOOLS;
        }

        public void updateForceFeedback(bool newState)
        {
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.SET_HAPTIC_FORCES).ToString() + SEPARATOR + "1" + SEPARATOR + newState.ToString().ToLower();
            state = ViRTSA_FUNCTION_CODE.SET_HAPTIC_FORCES;
        }

        public void resetCubesPositions()
        {
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.RESET_OBJECTS_POSITIONS).ToString() + SEPARATOR + "0";
            state = ViRTSA_FUNCTION_CODE.RESET_OBJECTS_POSITIONS;

            // make cubes follow the chai3d position/orientation
            foreach (var c in cubesManagers)
            {
                c.SetFollowServerTransform(true);
            }
        }

        public void resetCubePosition(int c)
        {
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.RESET_OBJECT_POSITION).ToString() + SEPARATOR + "1" + SEPARATOR + c.ToString();
            state = ViRTSA_FUNCTION_CODE.RESET_OBJECT_POSITION;
        }

        public void setCubeDynamics(int c, bool v)
        {
            TCP_message = ((int)ViRTSA_FUNCTION_CODE.UPDATE_DYNAMICS).ToString() + SEPARATOR + "2" + SEPARATOR + c.ToString() + SEPARATOR + v.ToString().ToLower();
            state = ViRTSA_FUNCTION_CODE.UPDATE_DYNAMICS;
        }

        IEnumerator Ping()
        {
            while (performPing)
            {
                if (tcp_enabled && state == ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION && !waitingForAnswer)
                {
                    ping = true;
                }
                yield return new WaitForSeconds(20f);
            }
        }

        IEnumerator EnqueueTCPMessage()
        {
            yield return new WaitForSeconds(5f);
            while (true)
            {
                if (!tcp_enabled)
                {
                    state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                    waitingForAnswer = false;
                    ping = false;
                    TCP_message = "";
                    yield return new WaitForSeconds(.2f);
                }
                else
                {
                    if (state == ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION || waitingForAnswer)
                    {
                        if (!waitingForAnswer && ping)
                        {
                            Debug.Log("PING");
                            waitingForAnswer = true;
                            TCP_client.EnqueueMessage("0 0");
                            ping = false;
                        }
                        yield return new WaitForSeconds(.1f);
                    }
                    else
                    {
                        if (state == ViRTSA_FUNCTION_CODE.UPDATE_TOOLS)
                        {
                            if (!Int32.TryParse(numberOfDevStr, out numberOfDev))
                            {
                                numberOfDev = 0;
                            }
                            //SimulationManager.Instance.SetHapticDevicesUI(numberOfDevStr);    --FIXME
                            state = ViRTSA_FUNCTION_CODE.WAITING_FOR_INSTRUCTION;
                        }
                        else
                        {
                            ping = false;
                            Debug.Log("MESSAGE TO SEND: " + TCP_message);
                            waitingForAnswer = true;
                            TCP_client.EnqueueMessage(TCP_message);

                        }
                    }
                }
            }
        }
    }
}