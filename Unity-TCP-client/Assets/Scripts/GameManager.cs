using System;
using System.Collections;
using System.Net.Sockets;
using UnityEngine;
using FlatBuffers;
using Network;

public class GameManager : MonoBehaviour
{

    public GameObject target;

    private TcpClient client;
    private NetworkStream stream;

    private IEnumerator listenToNetwork()
    {
        ByteBuffer buffer;
        Packet packet;
        while(true)
        {
            try
            {
                byte[] bytes = new byte[1024];
                int bytesRead = stream.Read(bytes, 0, bytes.Length);

                buffer = new ByteBuffer(bytes);
                packet = Packet.GetRootAsPacket(buffer);

                Debug.Log(packet.Number);
                target.transform.position = new Vector3(3 * packet.Number, 0, 0);
            }
            catch (Exception e)
            {
                Debug.Log(e.ToString());
            }

            yield return null;
        }
    }
    // Start is called before the first frame update
    void Start()
    {
        try
        {
            client = new TcpClient("localhost", 9999);
            stream = client.GetStream();

            StartCoroutine(listenToNetwork());
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    // Update is called once per frame
    void Update()
    {

    }

    private void OnApplicationQuit()
    {
        StopCoroutine(listenToNetwork());
        try
        {
            client.Close();
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

}
