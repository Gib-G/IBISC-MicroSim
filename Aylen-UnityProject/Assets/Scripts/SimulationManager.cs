using System;
using System.Collections;
using System.IO;
using UnityEngine;
using ViRTSA.Networking;
using ViRTSA.Plugin;
using ViRTSA.UI;

[RequireComponent(typeof(ViRTSA.Networking.TCPClient))]
[RequireComponent(typeof(ViRTSA.Networking.UDPListener))]
[RequireComponent(typeof(ViRTSA.Plugin.VirtsaManager))]
public class SimulationManager : MonoBehaviour
{
    public static SimulationManager Instance;

    public EvaluationConfig Configuration;
    private EvaluationConfig newConfiguration = null;

    private TCPClient TCP_client;
    private UDPListener UDP_listener;
    private VirtsaManager ViRTSAManager;

    public MenuSession SessionUI;
    public MenuExperimentation MainUI;

    public int state = -1;

    private void Awake()
    {
        // Keep this game object through all scenes
        UnityEngine.Object.DontDestroyOnLoad(gameObject);

        // Save singleton Instance
        if (Instance == null)
            Instance = this;

        // Load Configuration for the simulation (TCP,UDP and COM info)
        Configuration = EvaluationConfig.Load(Path.Combine(Definitions.CONFIG_PATH, Definitions.CONFIG_EVALUATION_FILE));
    }

    // Start is called before the first frame update
    void Start()
    {
        state = 0;

        // TCP server reference
        try
        {
            TCP_client = GetComponent<TCPClient>();
        }
        catch (Exception)
        {
            TCP_client = gameObject.AddComponent<TCPClient>();
        }

        // UDP listener reference
        try
        {
            UDP_listener = GetComponent<UDPListener>();
        }
        catch (Exception)
        {
            UDP_listener = gameObject.AddComponent<UDPListener>();
        }

        // Main UI reference
        if (MainUI == null)
        {
            MainUI = GameObject.FindObjectOfType<MenuExperimentation>();
        }
        if (MainUI != null)
        {
            MainUI.Setup();
        }

        // Session UI reference
        if (SessionUI == null)
        {
            SessionUI = GameObject.FindObjectOfType<MenuSession>();
        }

        // ViRTSA Manager reference
        try
        {
            ViRTSAManager = GetComponent<VirtsaManager>();
        }
        catch (Exception)
        {
            Debug.LogError("VERIFY THAT VirtsaManager is properly configured (cubes and tools receivers scripts added)");//ViRTSAManager = gameObject.AddComponent<VirtsaManager>();
            state = 1;
            return;
        }
        ViRTSAManager.SetTCPClient(TCP_client);
        ViRTSAManager.SetUDPClient(UDP_listener);
    }

    // Update is called once per frame
    void Update()
    {

    }

    public void SendInformationForNewSimulation(/*UserInformation currentUser,*/ EvaluationConfig appConfig)
    {
        if (state == 1) return;
        state = 1;

        //*CurrentUser = currentUser;
        Configuration = appConfig;

        // 
        if (SessionUI != null)
        {
            SessionUI.Setup();
        }

        // connect to the server (TCP -> IP port)
        ViRTSAManager.NewSimulation();
    }

    // **********************
    //  UI buttons callbacks
    // **********************

    public void SetHapticDevicesUI(string number)
    {
        SessionUI.UpdateDevicesCount(number);
    }

    public void SetTCPServerUI(bool value)
    {
        SessionUI.UpdateTCPFeedback(value);
    }

    IEnumerator StartVirtsaDelay(float seconds)
    {
        yield return new WaitForSeconds(seconds);
        ViRTSAManager.StartVirtsaChai3d();
    }

    public void StartVirtsaApp()
    {
        ViRTSAManager.StartVirtsaChai3d();
        SessionUI.UpdateButtons(true);
    }

    public void StopApplication()
    {
        ViRTSAManager.StopApplication();
        SessionUI.UpdateButtons(false);
    }

    public void ReconnectTCPServer(bool sendStartApp)
    {
        // update the configuration if changes have been made on the configuration panel of the ExpPanel
        if (newConfiguration != null)
        {
            Configuration = newConfiguration;
            newConfiguration = null;
        }

        ViRTSAManager.ReconnectTCPServer();
        if (sendStartApp)   // toggle activated
            StartCoroutine("StartVirtsaDelay", 2.0f);
    }

    public void SendInfoToSimulatorManager(EvaluationConfig appConfig)
    {
        newConfiguration = appConfig;
    }

    public void DisconnectTCPServer()
    {
        ViRTSAManager.DisconnectTCPServer();
    }

    public void updateForceFeedback(bool newState)
    {
        ViRTSAManager.updateForceFeedback(newState);
    }

    public void resetCubesPositions()
    {
        ViRTSAManager.resetCubesPositions();
    }

    internal void resetCubePosition(int cube)
    {
        ViRTSAManager.resetCubePosition(cube);
    }

    internal void updateCubeDynamics(int cube, bool enable)
    {
        ViRTSAManager.setCubeDynamics(cube, enable);
    }
}
