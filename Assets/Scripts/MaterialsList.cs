using UnityEngine;
using System.Collections;

public class MaterialsList : MonoBehaviour {

    public Material[] materials;
    
    public  Material GetMaterial(int index)
    {
        return materials[index];
    }

}
