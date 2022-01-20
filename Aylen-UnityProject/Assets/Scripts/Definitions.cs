using System.Collections.Generic;
using System.IO;
using UnityEngine;

public enum ExperimentCondition : int
{
    HI5_HAND = 0,
    NO_HAND = 1
}

public enum GroupType : int
{
    CONTROL = 0,
    EXPERIMENTAL = 1
}

public enum SessionType : int
{
    PRE_TEST = 0,
    POST_TEST = 1,
    RETENTION_TEST = 2,
    TRAINING = 3
}

public enum Language : int
{
    EN = 0,
    ES = 1,
    FR = 2
}

public enum UITexts : int
{
    GENERIC,
    PARTICIPANT,
    GROUP,
    CONDITION,
    START,
    CONFIGURATION,
    LANGUAGE,
    SERIAL_TITLE,
    COM_PORT,
    BAUDRATE,
    SERVER_TITLE,
    IP_ADDRESS,
    TCP_PORT,
    UDP_PORT,
    CALIBRATION,
    LOGTEXT,
    DEBUG,
    LEFT,
    RIGHT,
    MALE,
    FEMALE,
    ENGLISH,
    SPANISH,
    FRENCH,
    HI5_HAND,
    NO_HAND,
    CONTROL_GROUP,
    EXPERIMENTAL_GROUP,
    HELP_PARTICIPANT,
    PRE_TEST,
    POST_TEST,
    RETENTION_TEST,
    TRAINING_SESSION,
    SESSION,
    MY_IP
}

public static class Definitions
{
    /// <summary>
    /// Media resources path
    /// </summary>
    public static string RESOURCES_PATH = Application.dataPath;

    public static string PARTICIPANT_FOLDER = "Participant";

    /// <summary>
    /// Config files path
    /// </summary>
    public static string CONFIG_PATH = Path.Combine(Application.dataPath, "..\\Config\\");

    public static string DEBUG_OUTPUT_PATH = Path.Combine(Application.dataPath, "..\\DebugOutput\\");

    /// <summary>
    /// Config files path
    /// </summary>
    public static string LOG_PATH = Path.Combine(Application.dataPath, "..\\Log\\");

    public static string CALIBRATION_FOLDER = "Calibrations";

    /// <summary>
    /// Scene config file
    /// </summary>
    public static string CONFIG_EVALUATION_FILE = "evaluation.xml";
    public static string LOG_EVALUATION_FILE = "evaluation";

    /// <summary>
    /// 
    /// </summary>
    public static string CONFIG_USER_FILE = "user.xml";
    public static string LOG_USER_FILE = "user";

    /// <summary>
    /// 
    /// </summary>
    public static string CONFIG_USERS_FILE = "users.xml";

    /// <summary>
    /// 
    /// </summary>
    public static string CONFIG_UIITEMS_FILE = "UIitems.xml";

    /// <summary>
    /// One millisecond constant
    /// </summary>
    public const float ONE_MILLISECOND = 1.0f;

    /// <summary>
    /// One second constant
    /// </summary>
    public const float ONE_SECOND = 1000.0f;

    /// <summary>
    /// Shuffles a list randomly
    /// </summary>
    /// <typeparam name="T">Type of the list elements</typeparam>
    /// <param name="list">List to shuffle</param>
    /// <remarks>Modifies the calling list object</remarks>
    public static void Shuffle<T>(this IList<T> list)
    {
        System.Random rng = new System.Random();
        int n = list.Count;
        while (n > 1)
        {
            n--;
            int k = rng.Next(n + 1);
            T value = list[k];
            list[k] = list[n];
            list[n] = value;
        }
    }
}
