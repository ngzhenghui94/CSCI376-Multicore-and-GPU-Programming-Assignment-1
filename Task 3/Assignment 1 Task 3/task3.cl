__kernel void fillArray(int a,   //index 0 inputMultiplier
						__global int *b) {     //index 1 in/output array
	
	int i = get_global_id(0); //0 - 1024 work items
	int arry[1024];
	arry[i] = i;  
	b[i] = (arry[i] * a) + 1;

}
