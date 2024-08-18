

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to store a number along with its length in byte
struct number{
	int len;
	unsigned char *num;
};

// Function to create a dyamically allocated array to store a number
struct number makeInt(int SIZE){
	struct number a;
    a.num = (unsigned char*)malloc(SIZE);
	memset(a.num,0,SIZE);
	a.len = SIZE;
    return a;
}

// Function to convert a hexadecimal string to an array of unsigned char
int hextoint(char hex) {
	//Conversion of hex characters to decimal value 
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    }
    return -1; // Invalid hexadecimal character
}
struct number hexToInt(char* hex,int size){
	struct number binInt= makeInt(size);
	for (int i=1; i<= size; i++) // Ignore the first byte containing 0x
		binInt.num[i-1]= (hextoint(hex[2*i])<<4) + hextoint(hex[2*i+1]);
	
	return binInt;
}

// Function to get the values from the file
void getInt(char* filename, struct number* num1, struct number* num2){
    char ch;
    int size[2]={0,0}; // Store the length of two numbers temporarily
    int count =0;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        printf("Unable to open the file.\n");
    else {
		while (fscanf(file, "%*s %d", &size[count]) == 1){
    		count++;
    	}
		rewind(file);	
		char buf1[2*size[0]+2];
		fscanf(file,"%s %*d", buf1);
		char buf2[2*size[1]+2];
		fscanf(file,"%s %*d", buf2);
		*num1= hexToInt(buf1,size[0]);
		*num2= hexToInt(buf2,size[1]);
		fclose(file);		
	}
}

// Function to print an integer stored in an array
void printInt(struct number integer, char* filename){
	FILE *file  = fopen(filename, "w");
    int flag=0;
    if (file != NULL) {
	    for(int i=0; i<integer.len; ++i){
	        if(flag!=0 || integer.num[i]!=0){
	            fprintf(file,"%02x",integer.num[i]);
	            flag=1;
	        }
		}
		fclose(file);
    }
    else {
        printf("Error opening file!\n");
    }
}

// Function to pad zeros to the beginning of the array
void padZero(struct number* integer, int targetLen) {
    int diff = targetLen - integer->len;
    struct number temp = makeInt(targetLen);
    memcpy(temp.num+diff,integer->num, integer->len);
    free(integer->num);
    *integer=temp;
}

// Function to make two arrays of equal length
void eqLen(struct number* num1, struct number* num2) {
    int l1=num1->len;
    int l2=num2->len;
    if (l1 < l2)
        padZero(num1,l2);
    else if (l1 > l2)
        padZero(num2,l1);
}

// Function to add two integers
struct number add(struct number* num1, struct number* num2) {
    int carry = 0;
    eqLen(num1,num2);
    int l = num1->len;
    struct number result= makeInt(l);
    for (int i = l-1; i >= 0; --i) {
        int sum = num1->num[i] + num2->num[i] + carry;
        result.num[i] = sum;
        carry = sum >> 8;
    }   
    if(carry != 0){
    	padZero(&result,l+1);
    	result.num[0]=1;
    }
    return result;
}

// Function to subtract two intgers, using 2's complement
struct number sub(struct number* num1, struct number* num2) {
	eqLen(num1,num2);
	int l = num1->len;
	struct number complement = makeInt(l);
	for (int i=0; i<l;++i)
		complement.num[i]= num2->num[i] ^ 0xff; // 1's complement of the subtrahend
	struct number just1 = makeInt(1);
	just1.num[0]=1;
	complement = add(&complement,&just1); // 2's complement of the subtrahend
	struct number result;
	result=add(num1,&complement);
	//Ignore the carry bit(if there is any)
    memmove(result.num, result.num + 1 , l);
    result.len--;
	//Free allocated memory
	free(just1.num);
	free(complement.num);
	return result;
}

// Function to multiply two integers
struct number karatsuba(struct number* x, struct number* y) {
    eqLen(x,y); //Make x & y of equal length
    int len = x->len;    
    // Base case: If both number is of single byte
    if(len==1){
    	struct number result= makeInt(1);
       	int product= x->num[0] * y->num[0];
       	int carry=0;
       	result.num[0] = product;
       	carry = product >> 8;
        if(carry != 0){
			padZero(&result,2);
    		result.num[0]=carry;
  	    }
    	return result;
	} else {
    	// Split the input numbers into two halves
    	int n2 = len/2;
    	struct number xL = makeInt(n2);
    	struct number xR = makeInt(len - n2);
    	struct number yL = makeInt(n2 );
    	struct number yR = makeInt(len - n2);
    	memcpy(xL.num, x->num, n2);
    	memcpy(xR.num, x->num + n2, len - n2);
    	memcpy(yL.num, y->num, n2);
    	memcpy(yR.num, y->num + n2, len - n2);
    	
    	// Perform the recursive steps
    	struct number P1 = karatsuba(&xL, &yL);
    	struct number P2 = karatsuba(&xR, &yR);
    	struct number P1P2 = add(&P1,&P2);
    	struct number xLR = add(&xL, &xR);
    	struct number yLR = add(&yL, &yR);
    	struct number P3 = karatsuba(&xLR, &yLR);
    	struct number P4 = sub(&P3,&P1P2);
    	
    	// Get the final result from P1, P2, P3, P4
    	struct number result = makeInt((2*len));
    	padZero(&P1,2*n2);
    	memcpy(result.num, P1.num, 2*n2);
    	result= add(&result,&P2);
    	int l = P4.len;
    	for (int i=0; i<len-n2; i++)
    		P4.num[l+i]=0;
    	P4.len = l+len-n2;
    	result= add(&result,&P4);
    
    // Free dynamically allocated memory
    free(xL.num);
    free(xR.num);
    free(yL.num);
    free(yR.num);
    free(P1.num);
    free(P2.num);
    free(P3.num);
    free(P4.num);
    free(P1P2.num);
    free(xLR.num);
    free(yLR.num);
    return result;
    }
}

int main(){
	char infile[100]; // Maximum filename length assumed to be 100 characters
	char outfile[100]; // Maximum filename length assumed to be 100 characters
    printf("Please enter the filename where the integers are stored : ");
    scanf("%s", infile);
    printf("Please enter the filename where the result is to be stored : ");
    scanf("%s", outfile);
    struct number num1, num2, result;
	getInt(infile,&num1, &num2);
	result= karatsuba(&num1,&num2);
	printInt(result, outfile);
	//karatsuba(num1,num2);
	printf("Result has been written to %s \n", outfile);
	free(num1.num);
	free(num2.num);
	free(result.num);
	return 0;
} 
