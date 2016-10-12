using UnityEngine;
using System.Collections;

public class Main : MonoBehaviour {

    public Branch initiator;

    public delegate void TriggerBranchesDelegate();
    public TriggerBranchesDelegate triggerBranchesEvent;
	
	void Update ()
    {
	    if (Input.GetKeyDown(KeyCode.Space))
        {
            // Process triggerBranchesEvent subscribers - triggers their next set of branches.
            if (triggerBranchesEvent != null) { triggerBranchesEvent(); }
        }
        if (Input.GetKeyDown(KeyCode.C))
        {
            Debug.Log(Branch.GetBranchCount());
        }
	}

}
