using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

public class FVM : MonoBehaviour
{
	float dt 			= 0.003f;
    float mass 			= 1;
	float stiffness_0	= 20000.0f;
    float stiffness_1 	= 5000.0f;
    float damp			= 0.999f;

	int[] 		Tet;
	int tet_number;			//The number of tetrahedra

	Vector3[] 	Force;
	Vector3[] 	V;
	Vector3[] 	X;
	int number;				//The number of vertices

	Matrix4x4[] inv_Dm;

	//For Laplacian smoothing.
	Vector3[]   V_sum;
	int[]		V_num;

	SVD svd = new SVD();

    // Start is called before the first frame update
    void Start()
    {
    	// FILO IO: Read the house model from files.
    	// The model is from Jonathan Schewchuk's Stellar lib.
    	{
    		string fileContent = File.ReadAllText("Assets/house2.ele");
    		string[] Strings = fileContent.Split(new char[]{' ', '\t', '\r', '\n'}, StringSplitOptions.RemoveEmptyEntries);
    		
    		tet_number=int.Parse(Strings[0]);
        	Tet = new int[tet_number*4];

    		for(int tet=0; tet<tet_number; tet++)
    		{
				Tet[tet*4+0]=int.Parse(Strings[tet*5+4])-1;
				Tet[tet*4+1]=int.Parse(Strings[tet*5+5])-1;
				Tet[tet*4+2]=int.Parse(Strings[tet*5+6])-1;
				Tet[tet*4+3]=int.Parse(Strings[tet*5+7])-1;
			}
    	}
    	{
			string fileContent = File.ReadAllText("Assets/house2.node");
    		string[] Strings = fileContent.Split(new char[]{' ', '\t', '\r', '\n'}, StringSplitOptions.RemoveEmptyEntries);
    		number = int.Parse(Strings[0]);
    		X = new Vector3[number];
       		for(int i=0; i<number; i++)
       		{
       			X[i].x=float.Parse(Strings[i*5+5])*0.4f;
       			X[i].y=float.Parse(Strings[i*5+6])*0.4f;
       			X[i].z=float.Parse(Strings[i*5+7])*0.4f;
       		}
    		//Centralize the model.
	    	Vector3 center=Vector3.zero;
	    	for(int i=0; i<number; i++)		center+=X[i];
	    	center=center/number;
	    	for(int i=0; i<number; i++)
	    	{
	    		X[i]-=center;
	    		float temp=X[i].y;
	    		X[i].y=X[i].z;
	    		X[i].z=temp;
	    	}
		}
        /*tet_number=1;
        Tet = new int[tet_number*4];
        Tet[0]=0;
        Tet[1]=1;
        Tet[2]=2;
        Tet[3]=3;

        number=4;
        X = new Vector3[number];
        V = new Vector3[number];
        Force = new Vector3[number];
        X[0]= new Vector3(0, 0, 0);
        X[1]= new Vector3(1, 0, 0);
        X[2]= new Vector3(0, 1, 0);
        X[3]= new Vector3(0, 0, 1);*/


        //Create triangle mesh.
       	Vector3[] vertices = new Vector3[tet_number*12];
        int vertex_number=0;
        for(int tet=0; tet<tet_number; tet++)
        {
        	vertices[vertex_number++]=X[Tet[tet*4+0]];
        	vertices[vertex_number++]=X[Tet[tet*4+2]];
        	vertices[vertex_number++]=X[Tet[tet*4+1]];

        	vertices[vertex_number++]=X[Tet[tet*4+0]];
        	vertices[vertex_number++]=X[Tet[tet*4+3]];
        	vertices[vertex_number++]=X[Tet[tet*4+2]];

        	vertices[vertex_number++]=X[Tet[tet*4+0]];
        	vertices[vertex_number++]=X[Tet[tet*4+1]];
        	vertices[vertex_number++]=X[Tet[tet*4+3]];

        	vertices[vertex_number++]=X[Tet[tet*4+1]];
        	vertices[vertex_number++]=X[Tet[tet*4+2]];
        	vertices[vertex_number++]=X[Tet[tet*4+3]];
        }

        int[] triangles = new int[tet_number*12];
        for(int t=0; t<tet_number*4; t++)
        {
        	triangles[t*3+0]=t*3+0;
        	triangles[t*3+1]=t*3+1;
        	triangles[t*3+2]=t*3+2;
        }
        Mesh mesh = GetComponent<MeshFilter> ().mesh;
		mesh.vertices  = vertices;
		mesh.triangles = triangles;
		mesh.RecalculateNormals ();


		V 	  = new Vector3[number];
        Force = new Vector3[number];
        V_sum = new Vector3[number];
        V_num = new int[number];
        inv_Dm = new Matrix4x4[tet_number];
		//TODO: Need to allocate and assign inv_Dm
        for(int i = 0; i < tet_number; i++)
        {
            Vector3 x10 = X[Tet[i * 4 + 1]] - X[Tet[i * 4 + 0]];
            Vector3 x20 = X[Tet[i * 4 + 2]] - X[Tet[i * 4 + 0]];
            Vector3 x30 = X[Tet[i * 4 + 3]] - X[Tet[i * 4 + 0]];
            Matrix4x4 tt = new Matrix4x4(new Vector4(x10.x, x10.y, x10.z,0.0f),new Vector4(x20.x, x20.y, x20.z,0.0f),new Vector4(x30.x, x30.y, x30.z, 0.0f),new Vector4(0.0f,0.0f,0.0f,1.0f));
            inv_Dm[i] = tt.inverse;
        }
    }
    void Smooth()
    {
        //for every vertex,sum up all its neighbor's v --> blend it into its v.
        for(int i = 0; i < number; i++)
        {
            V_num[i] = 0;
            V_sum[i] = new Vector3(0.0f, 0.0f, 0.0f);
        }
        for (int tet = 0;tet < tet_number; tet++)
        {
            Vector3 sum = V[Tet[tet * 4 + 0]] + V[Tet[tet * 4 + 1]] + V[Tet[tet * 4 + 2]] + V[Tet[tet * 4 + 3]];
            V_sum[Tet[tet * 4 + 0]] += sum;
            V_sum[Tet[tet * 4 + 1]] += sum;
            V_sum[Tet[tet * 4 + 2]] += sum;
            V_sum[Tet[tet * 4 + 3]] += sum;
            V_num[Tet[tet * 4 + 0]] += 4;
            V_num[Tet[tet * 4 + 1]] += 4;
            V_num[Tet[tet * 4 + 2]] += 4;
            V_num[Tet[tet * 4 + 3]] += 4;
        }
        for(int i = 0; i < number; i++)
        {
            V[i] = 0.9f * V[i] + 0.1f * V_sum[i] / V_num[i];
        }
    }
    float trace(Matrix4x4 g)
    {
        float t = g[0,0] + g[1,1] + g[2,2];
        return t;
    }
    void _Update()
    {
    	// Jump up.
		if(Input.GetKeyDown(KeyCode.Space))
    	{
    		for(int i=0; i<number; i++)
    			V[i].y+=0.2f;
    	}

    	for(int i=0 ;i<number; i++)
    	{
            //TODO: Add gravity to Force.
            Force[i] = new Vector3(0.0f,-9.8f,0.0f) * mass;

    	}
    	for(int tet=0; tet<tet_number; tet++)
    	{
            Vector3 f1, f2, f3;
            //Build Edge Martix.
            Vector3 x10 = X[Tet[tet * 4 + 1]] - X[Tet[tet * 4 + 0]];
            Vector3 x20 = X[Tet[tet * 4 + 2]] - X[Tet[tet * 4 + 0]];
            Vector3 x30 = X[Tet[tet * 4 + 3]] - X[Tet[tet * 4 + 0]];
            Matrix4x4 x = new Matrix4x4(new Vector4(x10.x, x10.y, x10.z, 0.0f), new Vector4(x20.x, x20.y, x20.z, 0.0f), new Vector4(x30.x, x30.y, x30.z, 0.0f), new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            //TODO: Deformation Gradient
            Matrix4x4 F = Matrix4x4.zero;
            F = x * inv_Dm[tet];
            //TODO: Green Strain
            Matrix4x4 temp = F.transpose * F;
            Matrix4x4 G = temp;
            for (int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    if(i == j)
                    {
                        G[i, j] -= 1.0f;
                    }
                    G[i, j] *= 0.5f;
                }
            }

            //TODO: Second PK Stress
            float traceG = trace(G);
            Matrix4x4 S = Matrix4x4.zero;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    S[i, j] = 2.0f * stiffness_1 * G[i, j];
                    if(i == j)
                    {
                        S[i, j] += stiffness_0 * traceG * 1.0f;
                    }
                }
            }

            //Matrix4x4 S = 2.0f * stiffness_1 * G + stiffness_0 * trace(G) * Matrix4x4.identity;
            //TODO: Elastic Force
            Matrix4x4 P = F * S;
            Matrix4x4 DmT = inv_Dm[tet].transpose;
            Matrix4x4 force_v = Matrix4x4.zero;
            float c = 1.0f / (6.0f * inv_Dm[tet].determinant);
            force_v = P * DmT;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    force_v[i, j] *= -c;
                }
            }
            // Matrix4x4 force_v = -1.0f / (6.0f * inv_Dm[tet].determinant) * P * DmT;
            Vector4 temp_v = force_v.GetColumn(0);
            f1 = new Vector3(temp_v.x,temp_v.y,temp_v.z);
            temp_v = force_v.GetColumn(1);
            f2 = new Vector3(temp_v.x,temp_v.y, temp_v.z);
            temp_v = force_v.GetColumn(2);
            f3 = new Vector3(temp_v.x, temp_v.y, temp_v.z);
            Force[Tet[tet * 4 + 0]] -= f1 + f2 + f3 ; //f0
            Force[Tet[tet * 4 + 1]] += f1;
            Force[Tet[tet * 4 + 2]] += f2;
            Force[Tet[tet * 4 + 3]] += f3;
        } 

    	for(int i=0; i<number; i++)
    	{
            V[i] = damp * V[i];
            //TODO: Update X and V here.
            V[i] = V[i] + dt / mass * Force[i];
            X[i] = X[i] + dt * V[i];
            //TODO: (Particle) collision with floor.
           // Vector3 VV = X[i] - Plane;
            if(X[i].y < -3.0f)
            {
                if (V[i].y < 0.0f)
                //collision happended
                {
                    V[i].y = -V[i].y;
                }
            }
    	}
        Smooth();   //������ʽ���ֵĲ��ȶ���
    }

    // Update is called once per frame
    void Update()
    {
    	for(int l=0; l<10; l++)
    		 _Update();

    	// Dump the vertex array for rendering.
    	Vector3[] vertices = new Vector3[tet_number*12];
        int vertex_number=0;
        for(int tet=0; tet<tet_number; tet++)
        {
        	vertices[vertex_number++]=X[Tet[tet*4+0]];
        	vertices[vertex_number++]=X[Tet[tet*4+2]];
        	vertices[vertex_number++]=X[Tet[tet*4+1]];
        	vertices[vertex_number++]=X[Tet[tet*4+0]];
        	vertices[vertex_number++]=X[Tet[tet*4+3]];
        	vertices[vertex_number++]=X[Tet[tet*4+2]];
        	vertices[vertex_number++]=X[Tet[tet*4+0]];
        	vertices[vertex_number++]=X[Tet[tet*4+1]];
        	vertices[vertex_number++]=X[Tet[tet*4+3]];
        	vertices[vertex_number++]=X[Tet[tet*4+1]];
        	vertices[vertex_number++]=X[Tet[tet*4+2]];
        	vertices[vertex_number++]=X[Tet[tet*4+3]];
        }
        Mesh mesh = GetComponent<MeshFilter> ().mesh;
		mesh.vertices  = vertices;
		mesh.RecalculateNormals ();
    }
}
