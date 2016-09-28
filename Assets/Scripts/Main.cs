using UnityEngine;
using System.Collections;

public class Main : MonoBehaviour {

    private Branch initiator;

    public delegate void TriggerBranchesDelegate();
    public TriggerBranchesDelegate triggerBranchesEvent;

	void Start () {
        if (GameObject.FindObjectOfType<Branch>())
        {
            initiator = GameObject.FindObjectOfType<Branch>();
        }
        else
        {
            Debug.Log("No initiator branch found in scene. Please add one.");
        }
	}
	
	void Update () {
	    if (Input.GetKeyDown(KeyCode.Space))
        {
            // Call out to branches to trigger their next set of branches.
            if (triggerBranchesEvent != null) { triggerBranchesEvent(); }
        }
	}

}
