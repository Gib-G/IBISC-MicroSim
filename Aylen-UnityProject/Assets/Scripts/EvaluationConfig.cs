using System.IO;
using System.Xml.Serialization;

[XmlRoot("EvaluationConfig")]
public class EvaluationConfig
{
    public int TrainingSessionDuration;

    public string TCPserverAddr;
    public string UDPlisteningAddr;
    public int TCPport;
    public int UDPport;

    public int COMport;
    public int BaudRateSerial;

    /// <summary>
    /// Saves the UserInformation into an xml file
    /// </summary>
    /// <param name="path">Path to the xml file</param>
    public void Save(string path)
    {
        var serializer = new XmlSerializer(typeof(EvaluationConfig));
        using (var stream = new FileStream(path, FileMode.Create))
        {
            serializer.Serialize(stream, this);
        }
    }

    /// <summary>
    /// Loads the xml file into the UserInformation
    /// </summary>
    /// <param name="path">Path to the xml file</param>
    /// <returns>An instance of UserInformation containing the information loaded</returns>
    public static EvaluationConfig Load(string path)
    {
        var serializer = new XmlSerializer(typeof(EvaluationConfig));
        using (var stream = new FileStream(path, FileMode.Open))
        {
            return serializer.Deserialize(stream) as EvaluationConfig;
        }
    }
}
