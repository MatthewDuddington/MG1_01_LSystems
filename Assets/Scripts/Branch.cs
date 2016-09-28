using UnityEngine;
using System.Collections;

public class Branch : MonoBehaviour {

    public enum branchType { A1, A2, B1, B2 }
    private branchType myType;

    public Branch branchPrefab;

    private static MaterialsList branchMaterials;
    private static int A1Count = 0;
    private static int A2Count = 0;
    private static int B1Count = 0;
    private static int B2Count = 0;

    static private ArrayList branches;

    private Transform outSpawn;

    void Start ()
    {
        outSpawn = gameObject.GetComponentInChildren<OutSpawn>().transform;
        branchMaterials = GameObject.FindObjectOfType<MaterialsList>();

        // Listen for user triggering next wave of branch spawns.
        FindObjectOfType<Main>().triggerBranchesEvent += GenerateBranches;
    }

    public void GenerateBranches()
    {
        // Remove listener so that branches only spawn once from each branch.
        FindObjectOfType<Main>().triggerBranchesEvent -= GenerateBranches;

        switch (myType)
        {
            case branchType.A1:
                SpawnNewBranch(branchType.A2);
                SpawnNewBranch(branchType.B1);
                break;
            case branchType.A2:
                SpawnNewBranch(branchType.B2);
                SpawnNewBranch(branchType.A1);
                break;
            case branchType.B1:
                SpawnNewBranch(branchType.A1);
                break;
            case branchType.B2:
                SpawnNewBranch(branchType.A2);
                break;
        }
    } 

    private void SpawnNewBranch (branchType newType)
    {
        Branch newBranch = Instantiate(branchPrefab) as Branch;
        newBranch.transform.position = outSpawn.position;
        newBranch.transform.SetParent(transform);
        newBranch.SetupBranch(newType);
    }

    private void SetupBranch (branchType newType)
    {
        // Sets name, material and rotation of new branches based on their branchType.
        // Also regesters new branch to the appropriate counter.

        myType = newType;
        MeshRenderer myMesh = gameObject.GetComponentInChildren<MeshRenderer>();
        switch (myType)
        {
            case branchType.A1:
                A1Count++;
                gameObject.name = "A1 Branch " + A1Count;
                myMesh.material = branchMaterials.GetMaterial(0);
                transform.Rotate(Vector3.forward * 15);
                break;
            case branchType.A2:
                A2Count++;
                gameObject.name = "A2 Branch " + A2Count;
                myMesh.material = branchMaterials.GetMaterial(1);
                transform.Rotate(Vector3.back * 15);
                break;
            case branchType.B1:
                B1Count++;
                gameObject.name = "B1 Branch " + B1Count;
                myMesh.material = branchMaterials.GetMaterial(2);
                transform.Rotate(Vector3.forward * 15);
                break;
            case branchType.B2:
                B2Count++;
                gameObject.name = "B2 Branch " + B2Count;
                myMesh.material = branchMaterials.GetMaterial(3);
                transform.Rotate(Vector3.back * 15);
                break;
        }
        transform.localScale = (transform.localScale * 0.8f);
    }

}
