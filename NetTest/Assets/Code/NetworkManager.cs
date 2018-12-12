using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine.Tilemaps;

enum MessageID
{
    e_id_packetBegin = 134,

    ID_GAME_MESSAGE_1,
		UPDATE_NETWORK_PLAYER, // Client sends server a single entity
		UPDATE_GAME_STATE, // Server sends client update of all current entities
		PLAYER_COLLISION,
		PLAYER_WIN_ROUND,
		PLAYER_WIN_MATCH,

    // ****TO-DO: implement general identifiers
    // these should be peer-type-agnostic, i.e. either server or client
    // some suggestions: 
    //	e_id_stateInput,	// packet data contains remote input
    //	e_id_stateData,		// packet data contains info about game state
    // etc.

    // end; indicates where specialized managers can start
    e_id_packetEnd
}

public class NetworkManager : MonoBehaviour
{
    public bool enableNetworking;
    public SceneController sceneManager; // TODO: Replace with entity manager for final.
    public string ip = "127.0.0.1";

    [DllImport("egp-net-plugin-Unity")]
    private static extern int foo(int bar);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool initNetworking(IntPtr ip);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool disconnect();
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool shutdownNetworking();
    [DllImport("egp-net-plugin-Unity")]
    private static extern int handlePacket();
    [DllImport("egp-net-plugin-Unity")]
    private static extern IntPtr plsreturn(ref int length);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool sendEntityToServer(int guidSize, IntPtr guid, SimpleVector3 position, SimpleVector3 destination, float collisionRadius, bool inCombat, int currentAttack);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool sendCombatUpdateToServer(int guidSize, byte[] guid, SimpleVector3 position, SimpleVector3 destination, float collisionRadius, bool inCombat, int currentAttack, int opponentGuidLength, byte[] opponentGuid);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool getNextEntityUpdate(ref int guidLength, out IntPtr guid, ref SimpleVector3 position, ref SimpleVector3 destination, ref UInt64 latency);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool getNextCollisionUpdate(ref int guid1Length, out IntPtr guid1, ref int guid2Length, out IntPtr guid2);
    [DllImport("egp-net-plugin-Unity")]
    private static extern bool getNextRoundWinUpdate(ref int winnerGuidLength, out IntPtr winnerGuid, ref int loserGuidLength, out IntPtr loserGuid, ref bool draw);

    // We want to send data to the server 10 times a second
    private const float networkTickRateMS = 100.0f / 1000.0f;
    //private const float networkTickRateMS = 500.0f / 1000.0f; //Debug: twice a second
    private float lastNetworkUpdate;

    private int entityUpdatesWaiting;
    private int collisionUpdatesWaiting;
    private int roundWinUpdatesWaiting;

    // Start is called before the first frame update
    void Start()
    {
        if (enableNetworking)
        {
            lastNetworkUpdate = Time.time;
            IntPtr ipPtr = Marshal.StringToHGlobalAnsi(ip);
            if (initNetworking(ipPtr))
                Debug.Log("Connecting");
            else
                Debug.Log("Could not connect");
        }

        DontDestroyOnLoad(this);
    }

    // Update is called once per frame
    void Update()
    {
        if (enableNetworking)
        {
            // If it's been the time interval we send updates at
            //Debug.Log("Current Time: " + Time.time + " Last Update + Interval: " + lastNetworkUpdate + networkTickRateMS);
            if (Time.time >= lastNetworkUpdate + networkTickRateMS)
            {
                //Debug.Log("Network Update");
                // Reset the timer
                lastNetworkUpdate = Time.time;
                // Debug Test
                SendEntity(SceneController.localPlayer);
            }

            // Check for and queue packets as fast and often as we can.
            HandleNetworking();

            if (entityUpdatesWaiting > 0)
            {
                //TODO:
                // loop through all updates
                // if we have that entity, update it
                // if we don't, create it
                for (int i = 0; i < entityUpdatesWaiting; i++)
                { 
                    //Debug.Log("Update Network Player");

                    Guid entityGuid;
                    int guidLength = 0;
                    IntPtr guidReturn = IntPtr.Zero;
                    SimpleVector3 position = new SimpleVector3(0.0f,0.0f,0.0f);
                    SimpleVector3 destination = new SimpleVector3(0.0f, 0.0f, 0.0f);
                    UInt64 latency = 0;

                    if (!getNextEntityUpdate(ref guidLength, out guidReturn, ref position, ref destination, ref latency) || SceneController.localPlayer.inCombat)
                    {
                        continue;
                    }

                    byte[] guidBytes = new byte[guidLength];
                    Marshal.Copy(guidReturn, guidBytes, 0, guidLength);

                    entityGuid = bytesToGuid(guidBytes, 0, guidLength);

                    if (sceneManager)
                    {
                        EntityPacket newPacket;
                        newPacket.identifier = entityGuid;
                        newPacket.position = new Vector3(position.x, position.y, position.z);
                        newPacket.destination = new Vector3(destination.x, destination.y, destination.z);
                        newPacket.latency = 0.0f;

                        // Debug.Log(identifer);

                        SceneController.entityPackets.Enqueue(newPacket);
                    }
                    //
                    //EntityPacket newPacket;
                    //newPacket.identifier = identifer;
                    //newPacket.position = position;
                    //newPacket.destination = destination;
                    //newPacket.latency = 0.0f; // get latency from returnData
                    //
                    ////Debug.Log(identifer);
                    //
                    //if (sceneManager)
                    //{
                    //    SceneManager.entityPackets.Enqueue(newPacket);
                    //}
                    //
                    //Debug.Log("indexfinal: " + index);


                }
                entityUpdatesWaiting = 0;
            }
            if (collisionUpdatesWaiting > 0)
            {
                Debug.Log("We have " + collisionUpdatesWaiting + " collision updates.");
                for (int i = 0; i < collisionUpdatesWaiting; i++)
                {
                    Guid player1Guid;
                    int guid1Length = 0;
                    IntPtr guid1Return = IntPtr.Zero;
                    Guid player2Guid;
                    int guid2Length = 0;
                    IntPtr guid2Return = IntPtr.Zero;

                    if (!getNextCollisionUpdate(ref guid1Length, out guid1Return, ref guid2Length, out guid2Return) || SceneController.localPlayer.inCombat)
                    {
                        continue;
                    }

                    byte[] guid1Bytes = new byte[guid1Length];
                    Marshal.Copy(guid1Return, guid1Bytes, 0, guid1Length);

                    player1Guid = bytesToGuid(guid1Bytes, 0, guid1Length);

                    byte[] guid2Bytes = new byte[guid2Length];
                    Marshal.Copy(guid2Return, guid2Bytes, 0, guid2Length);

                    player2Guid = bytesToGuid(guid2Bytes, 0, guid2Length);

                    Guid opponentID;
                    if (player1Guid == SceneController.localPlayer.identifier)
                    {
                        opponentID = player2Guid;
                    }
                    else if (player2Guid == SceneController.localPlayer.identifier)
                    {
                        opponentID = player1Guid;
                    }
                    else
                        continue;

                    SceneController.SwitchToCombatScene(opponentID);

                    //TODO:
                    // if either of i's guids are our local player's
                    // set the other guid in rps manager and then
                    // switch to the combat scene
                    collisionUpdatesWaiting = 0;
                }
            }
            if (roundWinUpdatesWaiting > 0)
            {
                //TODO:
                // loop through all updates
                // if any of them are ours, pass that info to rps manager then
                // delete all other updates, we can only be in one combat
            }
        }
    }

    void SendEntity(Entity send)
    {
        byte[] guidBytes = send.identifier.ToByteArray();
        int guidSize = guidBytes.Length;

        IntPtr guidPtr = Marshal.AllocHGlobal(guidSize);
        Marshal.Copy(guidBytes, 0, guidPtr, guidSize);

        SimpleVector3 position;
        position.x = send.transform.position.x;
        position.y = send.transform.position.y;
        position.z = send.transform.position.z;

        SimpleVector3 destination;
        destination.x = send.moveDestination.x;
        destination.y = send.moveDestination.y;
        destination.z = send.moveDestination.z;

        //Debug.Log(send.collisionRadius);

        sendEntityToServer(guidSize, guidPtr, position, destination, send.collisionRadius, send.inCombat, 0);

        string guidstr = "";
        for (int i = 0; i < guidSize; i++)
        {
            guidstr += (char)guidBytes[i];
        }
        //Debug.Log(guidstr);
    }

    private void HandleNetworking()
    {
        int packetType = handlePacket();

        if (packetType == -1)
        {
            return;
        }
        else if (packetType == (int)MessageID.UPDATE_NETWORK_PLAYER)
        {
            entityUpdatesWaiting++;
        }
        else if (packetType == (int)MessageID.PLAYER_COLLISION)
        {
            collisionUpdatesWaiting++;
        }
        else if (packetType == (int)MessageID.PLAYER_WIN_ROUND)
        {
            roundWinUpdatesWaiting++;
        }
        else
        {
            Debug.Log("Unknown packet recieved: " + packetType);
        }
    }

    private ulong bytesToUInt64(byte[] data, int startIndex)
    {
        byte[] reversed = new byte[8];
        for (int i = 0; i < 8; i++)
        {
            reversed[i] = data[startIndex + 8 - i];
        }
        return BitConverter.ToUInt64(reversed, 0);
    }

    private float bytesToFloat(byte[] data, int startIndex)
    {
        byte[] reversed = new byte[4];
        for (int i = 0; i < 4; i++)
        {
            reversed[i] = data[startIndex + 3 - i];
        }
        return BitConverter.ToSingle(reversed, 0);
    }

    private Guid bytesToGuid(byte[] data, int startIndex, int length)
    {
        //Debug.Log("Guid Length: " + length);
        byte[] guidBytes = new byte[length];
        for (int i = 0; i < length; i++)
        {
            guidBytes[i] = data[startIndex + i];
        }

        Guid newGuid = new Guid(guidBytes);

        return newGuid;
    }

    private void logEntityPacket(EntityPacket e)
    {
        Debug.Log("Guid: " + e.identifier.ToString());
        Debug.Log("Position: " + e.position);
        Debug.Log("Destination: " + e.destination);
    }

    void OnApplicationQuit()
    {
        Debug.Log("Disconnect Status: " + disconnect());
        Debug.Log("NetClosing Status: " + shutdownNetworking());
    }
}

public struct EntityPacket
{
    public Guid identifier;
    public Vector3 position;
    public Vector3 destination;
    public float latency;
}

[StructLayout(LayoutKind.Sequential)]
struct SimpleVector3
{
    
    public SimpleVector3(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    public float x;
    public float y;
    public float z;
}