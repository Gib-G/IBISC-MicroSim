using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
//using ViRTSA.Timming;
//using ViRTSA.User;

namespace ViRTSA.UI
{
    public class MenuSession : MonoBehaviour
    {
        private EvaluationConfig appConfig;
     
        // Panels control
        public GameObject MainSessionPanel;
        private bool configurationState = false;
        public Button ConfigurationPanelButton;
        public GameObject configurationPanel;
        private bool controlState = true;
        public Button ControlPanelButton;
        public GameObject controlPanel;

        // UI objects
        public InputField ComPort;
        public InputField BaudRate;
        public InputField ServerIP;
        public InputField TCPPort;
        public InputField UDPPort;
        public InputField MyIP;
        public Button UpdateInformationButton;
        public Text HapticDevicesQty;

        // UI Feedback
        public GameObject OK_TCP;
        public GameObject OK_FORCE;
        public GameObject NO_TCP;
        public GameObject NO_FORCE;

        // UI Action Buttons
        public Button ConnectTCPButton;
        public Button DisconnectTCPButton;
        public Button StartApplicationButton;
        public Button StopApplicationButton;
        //public Button StartTaskButton;
        //public Button StartFamiliarizationButton;
        public Button ForceFeedbackButton;
        public Button ResetPositionsButton;

        // UI cubes
        public List<Button> cubesResetPosition;
        public List<Button> cubesEnableDynamics;
        public List<Button> cubesDisableDynamics;

        public Toggle TCPToggle;

        void Awake()
        {
            MainSessionPanel.SetActive(false);

            ConfigurationPanelButton.onClick.AddListener(
                delegate
                {
                    OnClickConfig();
                }
            );

            ControlPanelButton.onClick.AddListener(
                delegate
                {
                    OnClickControl();
                }
            );

            UpdateInformationButton.onClick.AddListener(
                delegate
                {
                    UpdateInformationParameters();
                }
            );

            ConnectTCPButton.onClick.AddListener(
                delegate
                {
                    ConnectTCPAction();
                }
            );

            DisconnectTCPButton.onClick.AddListener(
                delegate
                {
                    DisconnectTCPAction();
                }
            );

            StopApplicationButton.onClick.AddListener(
                delegate
                {
                    StopApplicationAction();
                }
            );

            StartApplicationButton.onClick.AddListener(
                delegate
                {
                    StartApplicationAction();
                }
            );

            /*StartTaskButton.onClick.AddListener(
                delegate
                {
                    StartTaskAction();
                }
            );

            StartFamiliarizationButton.onClick.AddListener(
                delegate
                {
                    StartFamiliarizationAction();
                }
            );*/

            ForceFeedbackButton.onClick.AddListener(
                delegate
                {
                    ForceFeedbackAction();
                }
            );

            ResetPositionsButton.onClick.AddListener(
                delegate
                {
                    ResetPositionsAction();
                }
            );

            for (int i = 0; i < cubesResetPosition.Count; i++)
            {
                int index = i;
                cubesResetPosition[index].onClick.AddListener(
                    delegate
                    {
                        ResetPosition(index);
                    }
                );
            }

            for (int i = 0; i < cubesEnableDynamics.Count; i++)
            {
                int index = i;
                cubesEnableDynamics[index].onClick.AddListener(
                    delegate
                    {
                        UpdateDynamics(index, true);
                    }
                );
            }

            for (int i = 0; i < cubesDisableDynamics.Count; i++)
            {
                int index = i;
                cubesDisableDynamics[index].onClick.AddListener(
                    delegate
                    {
                        UpdateDynamics(index, false);
                    }
                );
            }

            // hide all status feedback
            ResetFeedback();
        }

        public void SetLapTimer(string msg)
        {
            //Timer.timer.text = msg;
        }

        /// <summary>
        /// Once started simulation in MainPanel, ExpPanel comes visible
        /// This function sets the initial values in configuration and sets default UI information
        /// </summary>
        public void Setup()
        {
            // load application defaults
            appConfig = SimulationManager.Instance.Configuration;
            ComPort.text = appConfig.COMport.ToString();
            BaudRate.text = appConfig.BaudRateSerial.ToString();
            ServerIP.text = appConfig.TCPserverAddr.ToString();
            MyIP.text = appConfig.UDPlisteningAddr.ToString();
            TCPPort.text = appConfig.TCPport.ToString();
            UDPPort.text = appConfig.UDPport.ToString();

            configurationPanel.SetActive(false);
            configurationState = false;
            controlPanel.SetActive(true);
            controlState = true;
            ResetFeedback();
            MainSessionPanel.SetActive(true);
        }

        /// <summary>
        /// (re)initialization of visual feedback/buttons
        /// </summary>
        public void ResetFeedback()
        {
            OK_TCP.SetActive(false);
            OK_FORCE.SetActive(false);
            NO_TCP.SetActive(false);
            NO_FORCE.SetActive(false);

            HapticDevicesQty.text = "0";

            foreach (var c in cubesResetPosition)
                c.gameObject.SetActive(true);
            foreach (var c in cubesEnableDynamics)
                c.gameObject.SetActive(true);
            foreach (var c in cubesDisableDynamics)
                c.gameObject.SetActive(false);
        }

        // ********************
        //  UI buttons actions
        // ********************

        public void OnClickConfig()
        {
            if (configurationPanel == null) return;

            configurationPanel.SetActive(!configurationState);
            configurationState = !configurationState;
        }

        public void OnClickControl()
        {
            if (controlPanel == null) return;

            controlPanel.SetActive(!controlState);
            controlState = !controlState;
        }

        // TODO see what it does!!
        public void UpdateInformationParameters()
        {
            // Update application default config
            int temp = -1;
            appConfig.TCPserverAddr = ServerIP.text;
            appConfig.UDPlisteningAddr = MyIP.text;
            appConfig.TCPport = Int32.TryParse(TCPPort.text, out temp) ? temp : -1; ;
            appConfig.UDPport = Int32.TryParse(UDPPort.text, out temp) ? temp : -1; ;
            appConfig.COMport = Int32.TryParse(ComPort.text, out temp) ? temp : -1; ;
            appConfig.BaudRateSerial = Int32.TryParse(BaudRate.text, out temp) ? temp : -1;

            //appConfig.Save(Path.Combine(Definitions.DEBUG_OUTPUT_PATH, Definitions.LOG_EVALUATION_FILE + "_" + SimulationManager.Instance.TimestampString + "_P" + currentUser.Identifier.ToString() + ".xml"));

            SimulationManager.Instance.SendInfoToSimulatorManager(appConfig);
        }

        // *************************************
        //  TCP server connection/disconnection
        // *************************************

        public void UpdateButtons(bool appStarted)
        {
            StartApplicationButton.interactable = !appStarted;
            StopApplicationButton.interactable = appStarted;
        }

        private void ConnectTCPAction()
        {
            bool value = TCPToggle.isOn;    // if toggle is on means: Connect Server + Start Application
            SimulationManager.Instance.ReconnectTCPServer(value);
            TCPToggle.isOn = false;
            ConnectTCPButton.interactable = false;
            DisconnectTCPButton.interactable = true;
        }

        private void DisconnectTCPAction()
        {
            SimulationManager.Instance.DisconnectTCPServer();
            ConnectTCPButton.interactable = true;
            DisconnectTCPButton.interactable = false;
        }

        // ********************
        //  Simulation actions
        // ********************

        private void StartApplicationAction()
        {
            SimulationManager.Instance.StartVirtsaApp();
        }

        private void StopApplicationAction()
        {
            SimulationManager.Instance.StopApplication();
        }

        private void StartTaskAction()
        {
            //SimulationManager.Instance.StartTask();
        }

        private void StartFamiliarizationAction()
        {
            //SimulationManager.Instance.StartFamiliarization();
        }

        private void ForceFeedbackAction()
        {
            bool newState = !(bool)OK_FORCE.activeInHierarchy;
            SimulationManager.Instance.updateForceFeedback(newState);
            UpdateForceFeedback(newState);
        }

        private void ResetPositionsAction()
        {
            SimulationManager.Instance.resetCubesPositions();
        }

        private void ResetPosition(int cube)
        {
            Debug.Log("ResetPosition " + cube);
            SimulationManager.Instance.resetCubePosition(cube);
        }

        private void UpdateDynamics(int cube, bool enable)
        {
            Debug.Log("UpdateDynamics " + cube + " " + enable);
            cubesEnableDynamics[cube].gameObject.SetActive(!enable);
            cubesDisableDynamics[cube].gameObject.SetActive(enable);
            SimulationManager.Instance.updateCubeDynamics(cube, enable);
        }

        public void UpdateTCPFeedback(bool newState)
        {
            OK_TCP.SetActive(newState);
            NO_TCP.SetActive(!newState);

            ConnectTCPButton.interactable = !newState;
            DisconnectTCPButton.interactable = newState;
        }

        public void UpdateForceFeedback(bool newState)
        {
            OK_FORCE.SetActive(newState);
            NO_FORCE.SetActive(!newState);
        }

        public void UpdateDevicesCount(string d)
        {
            HapticDevicesQty.text = d;
        }
    }
}
