using UnityEngine;
using System.Collections;

public class thruster : MonoBehaviour {

  public Rigidbody myRigidbody;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	  if (Input.GetKey(KeyCode.Space))
    {
      myRigidbody.AddRelativeForce(10*(transform.up));
    }
    if (Input.GetKey(KeyCode.Q))
    {
      myRigidbody.AddRelativeTorque(0.2f * (Vector3.forward));
    }
    if (Input.GetKey(KeyCode.E))
    {
      myRigidbody.AddRelativeTorque(0.2f * (Vector3.back));
    }
    if (Input.GetKey(KeyCode.A))
    {
      myRigidbody.AddRelativeForce(2 *(transform.right));
    }
    if (Input.GetKey(KeyCode.D))
    {
      myRigidbody.AddRelativeForce(2 * (-transform.right));
    }
    if (transform.rotation.eulerAngles.y < -85 || transform.rotation.eulerAngles.y > 85)
    {
      transform.rotation = Quaternion.identity;
    }
	}
}
