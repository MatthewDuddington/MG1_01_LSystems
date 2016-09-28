using UnityEngine;
using System.Collections;

public class MaterialsList : MonoBehaviour {

    public Material[] materials = new Material[4];
    
    public  Material GetMaterial(int index)
    {
        return materials[index];
    }

}
