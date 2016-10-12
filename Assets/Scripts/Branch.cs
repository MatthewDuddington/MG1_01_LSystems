using UnityEngine;
using System.Collections;

public class Branch : MonoBehaviour {

    public enum branchType { A1, A2, B1, B2 }
    private branchType myType;

    // All public var as using FindObject is expensive for lots of calls.
    public Branch branchPrefab;
    public MaterialsList branchMaterials;
    public Main main;
    public MeshRenderer myMesh;

    private static int A1Count = 1; // Starts at 1 because one of these is the initiator which is pre spawned in the scene.
    private static int A2Count = 0;
    private static int B1Count = 0;
    private static int B2Count = 0;

    static private ArrayList branches;

    private Transform outSpawn;

    void Start ()
    {
        outSpawn = gameObject.GetComponentInChildren<OutSpawn>().transform;
        // branchMaterials = GameObject.FindObjectOfType<MaterialsList>(); // Ineffienct with many objects in scene. Do it via inspector.

        // Listen for user triggering next wave of branch spawns.

        // FindObjectOfType<Main>().triggerBranchesEvent += GenerateBranches; // Inefficient with many objects in scene. Do it via inspector.
        // main = FindObjectOfType<Main>(); // Do it via inspector.
        main.triggerBranchesEvent += GenerateBranches;
    }

    public void GenerateBranches()
    {
        // Remove listener so that branches only spawn once from each branch.
        main.triggerBranchesEvent -= GenerateBranches;

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
        // MeshRenderer myMesh = gameObject.GetComponentInChildren<MeshRenderer>();
        switch (myType)
        {
            case branchType.A1:
                A1Count++;
                gameObject.name = "A1 Branch " + A1Count;
                myMesh.material = branchMaterials.GetMaterial(0);
                transform.Rotate(Vector3.forward * 30);
                break;
            case branchType.A2:
                A2Count++;
                gameObject.name = "A2 Branch " + A2Count;
                myMesh.material = branchMaterials.GetMaterial(1);
                transform.Rotate(Vector3.back * 30);
                break;
            case branchType.B1:
                B1Count++;
                gameObject.name = "B1 Branch " + B1Count;
                myMesh.material = branchMaterials.GetMaterial(2);
                transform.Rotate(Vector3.forward * 30);
                break;
            case branchType.B2:
                B2Count++;
                gameObject.name = "B2 Branch " + B2Count;
                myMesh.material = branchMaterials.GetMaterial(3);
                transform.Rotate(Vector3.back * 30);
                break;
        }
        transform.localScale = new Vector3(0.8f, 1, 0.8f);
    }

    public static string GetBranchCount()
    {
        string countInfo;
        countInfo = "A1 = " + A1Count + ", A2 = " + A2Count + ", B1 = " + B1Count + ", B2 = " + B2Count + ", Total branches = " + (A1Count + A2Count + B1Count + B2Count);
        return countInfo;
    }
    
}
