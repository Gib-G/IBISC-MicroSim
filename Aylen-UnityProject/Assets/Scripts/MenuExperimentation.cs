using System;
using UnityEngine;
using UnityEngine.UI;
//using ViRTSA.User;

namespace ViRTSA.UI
{
    /// <summary>
    /// Manages everything related to the MainPanel UI, including config and edit session panels
    /// Lifecycle: 
    /// -> Running from the beginning of the application
    /// -> Setup function is called by SimulationManager so as to load all the required information
    /// -> once start button pressed and actions executed, MenuExperimentation is no longer needed
    /// </summary>
    public class MenuExperimentation : MonoBehaviour
    {
        //private UserInformation currentUser;
        private EvaluationConfig appConfig;
        private bool configurationState = true;    // visibility of the configuration panel

        public GameObject MainStartPanel;

        // UI objects
        public Button StartButton;
        public Button ConfigurationPanelButton;
        public GameObject configurationPanel;
        public Dropdown LanguageDropdown;
        public InputField ComPort;
        public InputField BaudRate;
        public InputField ServerIP;
        public InputField TCPPort;
        public InputField UDPPort;
        public InputField MyIP;

        void Awake()
        {
            StartButton.onClick.AddListener(
                delegate
                {
                    OnClickStart();
                }
            );

            ConfigurationPanelButton.onClick.AddListener(
                delegate
                {
                    OnClickConfig();
                }
            );

            // Beginning of Application:
            MainStartPanel.SetActive(true);                     // main menu active
            configurationPanel.SetActive(configurationState);   // configuration panel hidden
        }

        /// <summary>
        /// Called once on initialization.
        /// Loads the list of users' IDs from users.xml in config folder
        /// Loads the application information (IPs, ports, etc.) needed to launch the app from evaluation.xml in config folder
        /// </summary>
        public void Setup()
        {
            /*List<int> listOfIds;
            try
            {
                listOfIds = SimulationManager.Instance.UsersController.GetParticipantsIDs();
            }
            catch (System.Exception)
            {
                Debug.LogError("SimulatorManager not instantiated");
                return;
            }

            // make a dropdown of all users ids
            ParticipantIDDropdown.options.Clear();
            foreach (int id in listOfIds)
                ParticipantIDDropdown.options.Add(new Dropdown.OptionData(id.ToString()));
            ParticipantIDDropdown.RefreshShownValue();

            currentUser = SimulationManager.Instance.UsersController.GetDefaultUser();
            */

            // load application defaults
            appConfig = SimulationManager.Instance.Configuration;
            ComPort.text = appConfig.COMport.ToString();
            BaudRate.text = appConfig.BaudRateSerial.ToString();
            ServerIP.text = appConfig.TCPserverAddr.ToString();
            MyIP.text = appConfig.UDPlisteningAddr.ToString();
            TCPPort.text = appConfig.TCPport.ToString();
            UDPPort.text = appConfig.UDPport.ToString();

            configurationPanel.SetActive(configurationState);

            //This updates to english experimenter UI + loads default participant info
            LanguageDropdown.value = (int)Language.EN;
        }

        /// <summary>
        /// Called on start button pressed. It launches the application with the selected user
        /// </summary>
        public void OnClickStart()
        {
            /*//string timestamp = SimulationManager.Instance.TimestampString;

            // Create user with the information obtained from the UI options
            int id = -1;
            currentUser.Identifier = Int32.TryParse(ParticipantIDDropdown.captionText.text, out id) ? id : -1;

            currentUser.Group = (GroupType)Enum.Parse(typeof(GroupType), GroupDropdown.value.ToString());
            currentUser.SessionType = (SessionType)Enum.Parse(typeof(SessionType), SessionDropdown.value.ToString());
            currentUser.ToRunCondition = (ExperimentCondition)Enum.Parse(typeof(ExperimentCondition), ConditionDropdown.value.ToString());
            currentUser.LogInformation = LogToggle.isOn;

            // save debug info of user
            currentUser.Save(Path.Combine(Definitions.DEBUG_OUTPUT_PATH, Definitions.LOG_USER_FILE + "_" + timestamp + "_P" + currentUser.Identifier.ToString() + ".xml"));
            */

            // Update application default config
            int temp = -1;
            appConfig.TCPserverAddr = ServerIP.text;
            appConfig.UDPlisteningAddr = MyIP.text;
            appConfig.TCPport = Int32.TryParse(TCPPort.text, out temp) ? temp : -1; ;
            appConfig.UDPport = Int32.TryParse(UDPPort.text, out temp) ? temp : -1; ;
            appConfig.COMport = Int32.TryParse(ComPort.text, out temp) ? temp : -1; ;
            appConfig.BaudRateSerial = Int32.TryParse(BaudRate.text, out temp) ? temp : -1;

            // save debug info of application config
            //appConfig.Save(Path.Combine(Definitions.DEBUG_OUTPUT_PATH, Definitions.LOG_EVALUATION_FILE + "_" + timestamp + "_P" + currentUser.Identifier.ToString() + ".xml"));

            try
            {
                // start simulation
                SimulationManager.Instance.SendInformationForNewSimulation(/*currentUser,*/ appConfig);
                MainStartPanel.SetActive(false);
            }
            catch (System.Exception e)
            {
                //TODO show this on a UI panel of errors/warnings --> build mode
                Debug.LogError("EXCEPTION INIT:" + e.Message);
                Debug.Log("maybe: SimulatorManager not instantiated");
            }
        }

        /// <summary>
        /// Called by clicking on config button (upper-left corner of the MainPanel UI)
        /// </summary>
        public void OnClickConfig()
        {
            if (configurationPanel == null) return;

            configurationPanel.SetActive(!configurationState);
            configurationState = !configurationState;
        }
    }
}