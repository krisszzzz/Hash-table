
# Table of Contents
* [Introduction](#intro)
  * [What is this README about?](#about_readme)
  * [Why hash table?](#why_hash_table)
* [Build](#build)
* [Usage and documentation](#usage_n_doc)
* [Hash functions collisions test](#hash_funct)
  * [About hash functions](#about_hash_funct)
  * [Tested hash functions](#tested_funct)
  * [Results](#hash_results)
	* [Charts](#charts)
	* [Conclusion from the test](#hash_conclusion)
* [Optimizations](#opt)
  * [About testing](#about_test)
  * [Used functions for testing](#used_functions)
  * [List of optimizations](#list_of_opt)
    * [crc32 hash optimization](#crc32)
	* [AVX2 searching optimizations](#AVX2)
  * [Conclusion](#opt_results)


## Introduction <a name="intro"></a>

### What is this README about? <a name="about_readme"></a>
This README contains information about the popular data structure, the hash table.  
The hash table will be used as an example of how to optimize the code (see [List of optimizations](#list_of_opt)),  
as well as to explore some hash functions (see [Tested hash functions](#tested_funct))  

If you are only interested in **building** the project read [Build](#build).



### Why hash table? <a name="why_hash_table"></a>
* Because the hash table is very fast, quite convenient and simple.  
This data structure is often used in many programs. The basic operations on that data i.e. the insertion, deletion, and searching can be (if there are no collisions) performed in O(1) time.
* Hash table is an good example of how writing optimal code can *speed up* a program.  
Because of the simplicity of the hash table, and because of the frequency of calling hash table  
functions (if you don't call hash table functions often, it's not very clear why you use a hash table  
at all), in the program appear the so-called *hotspots* - places where the program spends the most time on execution. (see [About optimizations](#about_opt))  

* Hash table can be used to check hash function collisions (See [Hash functions collision test](#hash_funct))

The biggest problem (which is solved in various fairly simple ways) hash tables in the presence of collisions. 
I used the chain method to deal with collisions. In the original, it uses a doubly linked list, however, since I
didn't start optimizing everything in a very smart way, I decided that the stack would be a faster solution. Now
I'm almost sure that this does not affect performance. In general, this does not greatly affect the hash table
itself, however, there is simply no function for deleting an element in it, just because I used a stack. Maybe
in the future I will fix it



[See the wiki for more details](https://en.wikipedia.org/wiki/Hash_table)

## Build <a name="build"></a> 

~~~ 
git clone https://github.com/krisszzzz/Hash-table
~~~

After to that go to the git directory.  
Make build directory.  
~~~
mkdir build
cd build
~~~

Use cmake to build the project.  
There are 2 build options available: without optimizations and with.  
To build a project with optimizations set cmake variable HASH_TABLE_OPTIMIZED on:
~~~
cmake -DHASH_TABLE_OPTIMIZED=ON ..
~~~
To turn off optimizations:
~~~
cmake -DHASH_TABLE_OPTIMIZED=OFF ..
~~~

Cmake will create a static library *libhash_table.a*.  
Don't forget to include header, located in *include/hash_table.h* in your sources, specifying with a macro what type of library (with or without optimizations) you have built.
~~~c
#define HASH_TABLE_OPTIMIZED  
#include "/path/to/header/hash_table.h" 

/*  By default (without defining HASH_TABLE_OPTIMIZED) none 
    of optimization will be used.
    But you can manually define no optimization define
    #define HASH_TABLE_UNOPTIMIZED
*/

// your source code

~~~
After that use gcc (or any compiler) to link static library:
~~~
gcc your_source_files /path/to/lib/libhash_table.a
~~~

## Hash functions collisions test <a name="hash_funct"></a>

### About hash functions <a name="about_hash_funct"></a>
A hash table can be used to study hash function collisions.  
Since the indexing of a hash table is based on finding the hash of the data, we can check the uniformity of the "population" of the data.  

For an ideal hash function, the following conditions are met:
1. the hash function is deterministic, i.e. the same message results in the same hash value.
2. the hash value is quickly calculated for any message.
3. it is not possible to find a message that gives the given hash value.
4. it is impossible to find two different messages with the same hash value.
5. a small change in the message changes the hash so much that the new and old values ​​seem uncorrelated.



### About collisions test <a name="about_hash_collision"></a>
In simpler terms, the "population" of the data, that is, the number of elements that lie under one index in the hash table. As I said for ideal hash function it is not possible to find two different message with the smae hash value. But in real life every hash function has a so-called *collisions*.  

Collisions are different objects with the same hash. To detect them, we will take a large text and put each word into a hash table. Thus, we can check how evenly the hash function "distributes" the data in the hash table. A good hash function should do this as uniformly as possible.  

Visual illustration explaining what I was talking about:  

![Collisions](pictures/Chaining2.png)

I used Lord of the Rings books to test collisions. These books contain 469332 words (counted with C program).  

### Tested hash functions <a name="tested_funct"></a>

I researched the following functions on collisions:  
***Click to show code.***

<details>
	<summary>hash_ret_1()<a name="hash_ret_1"></a></summary>  
	
~~~c
// A simple function for the most visual example  
inline hash_t hash_ret_1(const void* key, size_t size) { return 1; }
~~~
</details>

<details>
	<summary>hash_first_char()</summary>  
	
~~~c

inline hash_t hash_first_char(const void* key, size_t size)
{
        char ret = *(const char*)key;
        return (hash_t)ret;
}
~~~
</details>


<details>
	<summary>hash_len_char()</summary>  
	
~~~c

inline hash_t hash_len(const void *key, size_t size) { return size; }
~~~
</details>


<details>
	<summary>hash_ascii_char()</summary>  
	
~~~c	
inline hash_t hash_ascii_sum(const void* key, size_t size)
{
        hash_t res = 0;
        const char* char_key = (const char*)key;
        
        for (size_t iter_count = 0; iter_count < size; ++iter_count)
        {
                res += (hash_t)char_key[iter_count];
        }

        return res;
}
~~~
</details>

<details>
	<summary>hash_ded32_char()</summary>  

~~~c
// My sensei hash function
inline hash_t hash_ded32(const void* key, size_t size)
{
        const char* char_key = (const char*)key;
        hash_t hash = char_key[0];

        int offset = sizeof(hash_t) * 8 - 1;

        for (size_t iter_count = 1; iter_count < size; ++iter_count) {
                hash = ((hash >> 1) | (hash << offset)) ^ (hash_t)char_key[iter_count];
        }

        return hash;
}

~~~
</details>


<details>
	<summary>MurmurHash() <a name="Murmur"></a></summary>  

~~~c

inline hash_t MurmurHash(const void* key, size_t data_size)
{
        
        const hash_t          first_hash_coefficient  = 0x5bd1e993;
        const hash_t          seed                    = 0;
        const int             binary_offset           = 24;
        hash_t                hash                    = seed ^ data_size;
        const unsigned char*  data                    = (const unsigned char *)key;

        hash_t                second_hash_coefficient = 0;

        while (data_size >= 4)
        {
                second_hash_coefficient  = data[0];
                second_hash_coefficient |= data[1] << 8;
                second_hash_coefficient |= data[2] << 16;
                second_hash_coefficient |= data[3] << 24;

                second_hash_coefficient *= first_hash_coefficient;
                second_hash_coefficient ^= second_hash_coefficient >> binary_offset;
                second_hash_coefficient *= first_hash_coefficient;

                second_hash_coefficient *= first_hash_coefficient;
                hash                    ^= second_hash_coefficient;


                data      += 4;
                data_size -= 4;
        }

        switch (data_size)
        {
		case 3: {                    
			hash ^= data[2] << 16;    // We use fallthrough, to mix hash
			__attribute__((fallthrough));
		}
		case 2: {
			hash ^= data[1] << 8;
			__attribute__((fallthrough));
		}
		case 1: {
			hash ^= data[0];
			hash *= first_hash_coefficient;
			__attribute__((fallthrough));
	}    
		case 0: {
			break;
		}  
		
		default: {
			PrettyPrint("Inccorect data_size");
			return -1;
		}

        };

        hash ^= (hash >> 13);
        hash *= first_hash_coefficient;
        hash ^= (hash >> 15);

        return hash;
}

~~~
</details>
<br>

See the [documentation](#doc) for more info.


### Results <a name="hash_results"></a>
#### Charts <a name="charts"></a>

In most of cases I used the hash table with 256 elements. Note that I start element count from 0.  

**1. hash_ret_1**

![hash_ret_1](pictures/hash_ret_1.png)

Well, as you already understood, the result is quite obvious.  
As you can see, the peak of collisions is reached at table elements with index 1.  

**2. hash_first_char**

![hash_first_char](pictures/hash_first_char.png)

This result is also obvious. The peak is observed at index 115, which corresponds to the ascii character "s".  
Note that at some points the number of collisions is 0. This is explained by the fact that there are letters on which words cannot be started at all

**3. hash_len**

![hash_len](pictures/hash_len.png)

The peak is observed at index 7. This is due to the fact that the average length of English words is about 5.2 letters. However, in fiction, this value is usually greater.

**4. hash_ascii_sum**

![hash_ascii_sum](pictures/hash_ascii_sum.png)

Well it look pretty good, but this is because the size of the hash table is chosen correctly.  
Here is what the same function looks like, but with a 5000 element hash table:  

![hash_ascii_sum_5000](pictures/hash_ascii_sum_5000.png)

As you can see, the function hash behaves horribly if there are many elements in the hash table. 

**5. hash_ded32**

![hash_ded32](pictures/hash_ded32.png)

This hash function is bad, because its distribution already at 256 elements is extremely uneven.

**6. MurmurHash**

![MurmurHash](pictures/MurmurHash.png)

The collision distribution at 256 elements looks very similar to *hash_ascii_sum*, but notice how the hash function behaves if the hash table size is 5000:

![MurmurHash5000](pictures/MurmurHash5000.png)

As you can see this hash function is much more uniform than *hash_ascii_sum*.  

#### Сonclusion from the test <a name="hash_conclusion"></a>
Of all the given hash functions, only this MurmurHash can be called good. However, keep in mind that a properly sized hash table can make bad hash functions relatively good.

## Optimizations <a name="opt"></a>
First, let's look at the execution time of functions in the absence of optimizations. The testing function will be carried out below. Note that I assume that the hash table will be used more often for element lookup purposes. Note: I search everywhere the word "7lenstr". This word is not in hash table, but it is to test searching by this word. Notice that not optimized version, version with AVX2 depends on lenght of searched string, but version with inlined assembler is not. About this optimizations you will see below. Everywhere the size of hash table is set to 2048.  

The testing function is *unit_test_search()*. (See [About testing](#about_test))  

**Hash table searching testing, not optimized, iteration count 500000000, searched word ="7lenstr"**  
![No_opt_initial](pictures/no_opt_initial.png)

As you can see it is good idea to start optimizing function *hash_table_search()* and after that *MurmurHash()*  

### About testing <a name="about_test"></a>
This time I still use the books "The Lord of the Rings" to see how fast and what functions work.
Everywhere I used hash table with size 2048.
Used instrument: [perf](https://perf.wiki.kernel.org/index.php/Main_Page).

Used gcc flags:
~~~shell
gcc -D NDEBUG -Ofast -mavx2 -march=native -ansi -std=gnu++2a -fcheck-new 
    -fsized-deallocation -fstack-check -fstrict-overflow
~~~

<details>
	<summary>info about my system</summary>

~~~
# hostname : archlinux
# os release : 5.17.4-arch1-1
# perf version : 5.17.gf443e374ae13
# arch : x86_64
# nrcpus online : 8
# nrcpus avail : 8
# cpudesc : Intel(R) Core(TM) i5-9300H CPU @ 2.40GHz
# cpuid : GenuineIntel,6,158,10
# total memory : 16261960 kB
# sibling sockets : 0-7
# sibling dies    : 0-7
# sibling threads : 0,4
# sibling threads : 1,5
# sibling threads : 2,6
# sibling threads : 3,7
# CPU 0: Core ID 0, Die ID 0, Socket ID 0
# CPU 1: Core ID 1, Die ID 0, Socket ID 0
# CPU 2: Core ID 2, Die ID 0, Socket ID 0
# CPU 3: Core ID 3, Die ID 0, Socket ID 0
# CPU 4: Core ID 0, Die ID 0, Socket ID 0
# CPU 5: Core ID 1, Die ID 0, Socket ID 0
# CPU 6: Core ID 2, Die ID 0, Socket ID 0
# CPU 7: Core ID 3, Die ID 0, Socket ID 0
# node0 meminfo  : total = 16261960 kB, free = 9847212 kB
# node0 cpu list : 0-7
# CPU cache info:
#  L1 Data                 32K [0,4]
#  L1 Instruction          32K [0,4]
#  L1 Data                 32K [1,5]
#  L1 Instruction          32K [1,5]
#  L1 Data                 32K [2,6]
#  L1 Instruction          32K [2,6]
#  L1 Data                 32K [3,7]
#  L1 Instruction          32K [3,7]
#  L2 Unified             256K [0,4]
#  L2 Unified             256K [1,5]
#  L2 Unified             256K [2,6]
#  L2 Unified             256K [3,7]
#  L3 Unified            8192K [0-7]
~~~
</details>	

### Used functions for testing <a name="used_functions"></a>
List of functions that will be used to test the hash table.  
Not all functions are collected here, but I will talk about all of them in the appropriate section.  

This is functions used to fill hash table with words.

<details>
	<summary>is_sign()</summary>
	
~~~c
int is_sign(const char character)
{
        switch(character) {
        case ' ':
        case '\n':
        case '.':
        case '!':
        case ',':
        case '?':
        case '\'':
        case '\"':
                return 1;
        default:
                return 0; 
        };
}

~~~

</details>

<details>
	<summary>word_length()</summary>

~~~c
// Function to count word length. Needed to populate a hash table with words
int word_length(const char* const string)
{
        if (string == nullptr) {
                PrettyPrint("Unexpected nullptr string\n"); // See documentain for log.h and log.cpp files
                // Used to more beautiful output the errors
                return -1;
        }

        int iter_count = 0;
		
        for (; string[iter_count] != '\0' && !is_sign(string[iter_count]); ++iter_count) ;

         return iter_count;
}


~~~
</details>

<details>
	<summary>skip_spaces_n_marks()</summary>
	
~~~c
// Skip spaces between words 
void skip_spaces_n_marks(char** string)
{
        if (string == nullptr) {
                PrettyPrint("Unexpected nullptr string (const char** string)\n");
                // Used to more beautiful output the errors
                // You can find description of PrettyPrint() in log.h and log.cpp documentations
                return;
        }

        if (*string == nullptr) {
                PrettyPrint("Unexpected nullptr *string (const char* string)\n");
                return;
        }
        
        while (is_sign(**string)) {
                (*string) += 1;
        }

}

~~~
</details>
</br>

Simple unit test for hash table searching speed

<details>
	<summary>unit_test_search()</summary>
	
~~~c

// Test hash table by opening file and populating a hash table with words
// Used in unoptimized version

int unit_test_search(hash_table* const table, const char* const file_name, const int max_iter_count)
{
        if (file_name == nullptr) {
                PrettyPrint("Unexpected nullptr file_name\n");
                // Used to more beautiful output the errors
                // You can find description of PrettyPrint() in log.h and log.cpp documentations
                return -1;
        }

        if (table == nullptr) {
                PrettyPrint("Unexpected nullptr table (hash_table* const)\n");
                return -1;
        }

        char* buffer = load_file(file_name);
        // Open file and load data to buffer
        // You can find description of load_file() in file_read.h documentation

        char* to_free = buffer;

        if (buffer == nullptr) {
                PrettyPrint("File loading error\n");
                return -1; 
        }

        skip_spaces_n_marks(&buffer);
        
        while (*buffer != '\0') {
                int word_len = word_length(buffer);
                add_element(table, buffer, word_len);
                // add word to hash table
                // You can see description of add_element() in hash_table_no_opt.h and in hash_table_opt.h
				
                buffer += word_len;
                skip_spaces_n_marks(&buffer);
        }

        free(to_free);
		
        const char* test_word = "this_word_not_in_hash_table";

        for (int iter_count = 0; iter_count < max_iter_count; ++iter_count) {
                hash_table_search(table, test_word, strlen(test_word), nullptr, nullptr);
        }



        return 0;
}

~~~

</details>	

<br>

Sometimes I will specify how many iterations will be carried out in a loop.

### List of optimizations <a name="list_of_opt"></a>

#### AVX2 searching optimizations <a name="AVX2"></a>

It makes sense to change the comparison and search for elements. For this we will use AVX2 instructions. Also I used some gcc attributes (*\_\_attribute__()* like *hot*, *nonull*, see [gcc functions attributes](https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html)).
As before, I used Lords of The Rings text.

So I have optimized the searching function. This is how unoptimized version look:
<details>
	<summary>hash_table_search()</summary>

~~~c
int hash_table_search(hash_table* const self,        const char* const element,
                      const size_t      elem_size,   int*              in_table_pos,
                      int*              in_stack_pos)
{
        if (self == nullptr) {
                PrettyPrint("Unexpected nullptr self (hash_table* const)\n");
                return NULLPTR_ERROR;
        }

        if (element == nullptr) {
                PrettyPrint("Unexpected nullptr element (const char* const)\n");
                return NULLPTR_ERROR;
        }

        hash_t elem_index                 = self->hash_function(element, elem_size) % self->size;
        stack_node* current_stack_elem = self->data[elem_index].elements;
        
        for (int iter_count = 0; iter_count < self->data[elem_index].size; ++iter_count) {
                if (strncmp(current_stack_elem[iter_count].data, element, elem_size) == 0 && !current_stack_elem[iter_count].data[elem_size]) {
                        if (in_table_pos && in_stack_pos) {
                                *in_table_pos = elem_index;
                                *in_stack_pos = iter_count;
                        }
                        
                        return 1;
                } 
        }
              
        return 0; 
}


~~~
</details>
</br>


This is AVX2 version of searching:
<details>
	<summary>hash_table_search()</summary>
	
~~~c
	int hash_table_search(hash_table *const __restrict self,
                      const char *const __restrict element,
                      const size_t                 elem_size,
                      int *const __restrict        in_table_pos,
                      int *const __restrict        in_stack_pos)
{
        alignas(32) char temp[MAX_WORD_SIZE] = {};

        hash_t elem_index              = MurmurHash(element, elem_size) % SIZE;
        stack_node* current_stack_elem = DATA(elem_index).elements;
        
        int size = DATA(elem_index).size;
        strncpy(temp, element, elem_size);

        __m256i to_cmp = _mm256_load_si256((__m256i*)temp);
        
        for (int iter_count = 0; iter_count < size; ++iter_count) {
                
                __m256i mask        = _mm256_cmpeq_epi8(to_cmp, STACK_DATA(iter_count));
                unsigned int i_mask = (unsigned int)_mm256_movemask_epi8(mask);
                if (i_mask == 0xFFFFFFFF) {
                        if (in_table_pos && in_stack_pos) {
                                *in_table_pos = elem_index;
                                *in_stack_pos = iter_count;
                        }

                        return 1;
                }

        }
        

        return 0;
}

~~~
</details>
</br>

The testing function is *unit_test_search()*. (See [About testing](#about_test))

**Hash table searching testing, not optimized, iteration count 500000000, searched word ="7lenstr"**  
![no_opt_time](pictures/no_opt_search_time.png)

**Hash table searching testing, AVX2 optimization, iteration count 500000000, searched word ="7lenstr"**  
![avx2_opt_time](pictures/avx2_opt_time.png)

The searching became about 1.6 faster.


### Inline assembler optimization
I rewrited the hash function using inline assembler provided by gcc. I use a jump table to optimize the hash function. 
Note that in order to use the jump table, I had to declare the labels outside of the assembler injection (this is done using the LABEL macro), since assembler injections do not allow declaring data with the -pie compiler option. To keep the code independent of position, I declared the jump table as a static variable. Unfortunately, the inline version of this function cannot be used in this case. If you know a better way to do this, please let the author know.  
Full code of function:  

<details>
	<summary>fast_hash_function()</summary>
	
~~~c

#define LABEL(label_name)                              \
        extern void label_name() asm(#label_name);
// Find the used in inline assembler code labels
LABEL(equal_1);
LABEL(equal_2);
LABEL(equal_3);
LABEL(equal_4);
LABEL(equal_5);
LABEL(equal_6);
LABEL(equal_7);
LABEL(equal_8);
LABEL(more_8);


hash_t fast_hash_function(const char* const __restrict data, const int size)
{
                // Jmp table depend on size ==
        static u_int64_t jmp_table[32] = {       
                (u_int64_t)equal_1, // == 1
                (u_int64_t)equal_2, // == 2
                (u_int64_t)equal_3, // == 3
                (u_int64_t)equal_3, // == 4
                (u_int64_t)equal_4, // == 5
                (u_int64_t)equal_5, // == 6 
                (u_int64_t)equal_6, // == 7
                (u_int64_t)equal_7, // == 8
                (u_int64_t)equal_8, // == 9
                (u_int64_t)more_8,  // == 10
                (u_int64_t)more_8,  // == 11
                (u_int64_t)more_8,  // == 12
                (u_int64_t)more_8,  // == 13
                (u_int64_t)more_8,  // == 14
                (u_int64_t)more_8,  // == 15
                (u_int64_t)more_8,  // == 16
                (u_int64_t)more_8,  // == 17
                (u_int64_t)more_8,  // == 18
                (u_int64_t)more_8,  // == 19
                (u_int64_t)more_8,  // == 20
                (u_int64_t)more_8,  // == 21
                (u_int64_t)more_8,  // == 22
                (u_int64_t)more_8,  // == 23
                (u_int64_t)more_8,  // == 24
                (u_int64_t)more_8,  // == 25
                (u_int64_t)more_8,  // == 26
                (u_int64_t)more_8,  // == 27
                (u_int64_t)more_8,  // == 28
                (u_int64_t)more_8,  // == 29
                (u_int64_t)more_8,  // == 30
                (u_int64_t)more_8,  // == 31
                (u_int64_t)more_8,  // == 32
       };
        
        u_int64_t hash = 0;
        
        asm (
            "push %%r9\n\t"
            "push %%r10\n\t"
            "push %%r11\n\t"
            "push %%rsi\n\t"
            "push %%rdi\n\t"
            "xor %%r11, %%r11\n\t"
            "xor %%r10, %%r10\n\t"
            "lea %0, %%rax\n\t"
            "jmp *(%%rax, %%rsi, 8)\n\t"
            "more_8:\n\t"
            "mov (%%rdi), %%r9d\n\t"
            "crc32 %%r9d, %%r10d\n\t"
            "add %%r10, %%r11\n\t"
            "sub $0x8, %%rdi\n\t"
            "sub $0x8, %%rsi\n\t"
            "jmp *(%%rax,%%rsi, 8)\n\t"
            
            "equal_8:\n\t"
            "mov (%%rdi), %%r9d\n\t"
            "crc32 %%r9d, %%r10d\n\t"
            "jmp adding\n\t"
            "equal_7:\n\t"
            "mov (%%rdi), %%r9d\n\t"
            "mov 0x4(%%rdi), %%ax\n\t"
            "xor %%r9d, %%eax\n\t"
            "mov 0x6(%%rdi), %%r9b\n\t"
			"xor %%r9b, %%al\n\t"
            "crc32 %%eax, %%r10d\n\t"
            "jmp adding\n\t"
            "equal_6:\n\t"
            "mov (%%rdi), %%r9d\n\t"
            "mov 0x4(%%rdi), %%r10d\n\t"
            "crc32 %%r9d, %%r10w\n\t"
            "jmp adding\n\t"
            "equal_5:\n\t"
            "mov (%%rdi), %%r9d\n\t"
            "mov 0x4(%%rdi), %%al\n\t"
            "xor %%r9d, %%eax\n\t"
            "crc32 %%eax, %%r10d\n\t"
            "jmp adding\n\t"
            "equal_4:\n\t"
            "mov (%%rdi), %%r9d\n\t"
            "crc32 %%r9d, %%r10d\n\t"
            "jmp adding\n\t"
            "equal_3:\n\t"
            "mov (%%rdi), %%r9w\n\t"
            "mov 0x2(%%rdi), %%r10b\n\t"
            "xor %%r10d, %%r9d\n\t"
            "crc32 %%r9w, %%r10d\n\t"
            "jmp adding\n\t"
            "equal_2:\n\t"
            "mov (%%rdi), %%r9w\n\t"
            "crc32 %%r9w, %%r10d\n\t"
            "jmp adding\n\t"
            "equal_1:\n\t"
            "mov (%%rdi), %%r9b\n\t"
            "crc32 %%r9b, %%r10d\n\t"
            
            "adding:\n\t"
            "add %%r10, %%r11\n\t"
            "and $0x7ff, %%r11\n\t"
            "mov %%r11, %1\n\t"
            "pop %%rdi\n\t"
            "pop %%rsi\n\t"
            "pop %%r11\n\t"
            "pop %%r10\n\t"
            "pop %%r9\n\t"
            :"=m"(jmp_table)
            :"r"(hash),
             "r"(data),
             "r"(size)
             );

        return hash;
 
}

~~~

</details>
</br>

The assembler code in intel syntax:  
<details>

<summary>Intel syntax assembler</summary>

~~~assembler

push rdx
push r9
push r10
push r11
push rsi
push rdi
xor r11, r11
xor r10, r10
mov rax jmp_table
jmp [rax + rsi*8]
more_8:
mov r9d, [rdi]
crc32 r10d, r9d
add r11, r10
sub rdi, 8
sub rsi, 8
jmp [rax + rsi*8]

equal_8:
mov r9d, [rdi]
crc32 r10d, r9d
jmp adding

equal_7:
mov r9, [rdi]
mov ax, [rdi + 4]
xor eax, r9d
mov r9b, [rdi + 6]
xor r9b, al
crc32 r10d, eax
jmp adding

equal_6:
mov r9d, [rdi]
mov r10w, [rdi+4]
crc32 r10d, r9d
jmp adding

equal_5:
mov r9d, [rdi]
mov r10b, [rdi+4]
crc32 r10d, r9d
jmp adding

equal_4:
mov r9d, [rdi]
crc32 r10d, r9d
jmp adding

equal_3:
mov r9w, [rdi]
mov r10b, [rdi+2]
xor r9d, r10d
crc32 r10d, r9w
jmp adding

equal_2:
mov r9w, [rdi]
crc32 r10d, r9w
jmp adding

equal_1:
mov r9b, [rdi]
crc32 r10d, r9b

adding:
add r11, r10
and r11, 0x7ff ; Division to 2048
mov [hash], r11
pop rdi
pop rsi
pop r11
pop r10
pop r9

~~~

</details>
</br>

**Hash table searching testing, AVX2 optimization, iteration count 500000000, searched word ="7lenstr"**  
![avx2_opt_time](pictures/avx2_opt_time.png)

**Hash table searching testing, AVX2 + inline assembler optimization, iteration count 500000000, searched word ="7lenstr"**  
![inline_asm_opt_time](pictures/inline_asm_opt_time.png)

The searching time is 1.2 times faster.

### Function rewriting to assembler
The last optimization is to use assembler and rewrite the function and statically link it to the project. I rewrite the avx version of hash table searching. The code look like this.  

<details>
	<summary>Intel syntax assembler code</summary>
	
~~~
hash_table_search_asm	
push   rbp
mov    rbp,rsp
push   r15
push   r14
push   r13
push   r12
push   rbx
and    rsp,0xffffffffffffffe0
sub    rsp,0x1000
or     QWORD PTR [rsp],0x0
sub    rsp,0x60
or     QWORD PTR [rsp],0x0
add    rsp,0x1020
vpxor  xmm0,xmm0,xmm0
mov    r9,rdi
mov    rdi,rsi
mov    esi,edx
mov    QWORD PTR [rsp+0x38],rax
xor    eax,eax
vmovdqa XMMWORD PTR [rsp],xmm0
vmovdqa XMMWORD PTR [rsp+0x10],xmm0
call   fast_hash_function 
mov    r12,rax
shl    rax,0x5
add    rax,QWORD PTR [r9+0x8]
mov    rbx,QWORD PTR [rax]
mov    r15d,DWORD PTR [rax+0xc]
mov    rax,rsp
mov    rsi,rdi
movsxd rdx,edx
mov    rdi,rax
mov    r13,rcx
mov    r14,r8
call   strncpy 
vmovdqa ymm1,YMMWORD PTR [rsp]
test   r15d,r15d
jle    Exit  
mov    rax,rbx
xor    edx,edx
jmp   .start_cmp 
nop    DWORD PTR [rax+rax*1+0x0]
.cycle:	
inc    edx
add    rax,0x20
cmp    r15d,edx
je     Exit
.start_cmp:
vpcmpeqb ymm0,ymm1,YMMWORD PTR [rax]
vpmovmskb ecx,ymm0
cmp    ecx,0xffffffff
jne    .cycle
test   r13,r13
je     is_null
test   r14,r14
je     is_null
mov    DWORD PTR [r13+0x0],r12d
mov    DWORD PTR [r14],edx
is_null:	
mov    eax,0x1
jmp    .skip_xor
.Exit:	
xor    eax,eax
.skip_xor:	
mov    rdx,QWORD PTR [rsp+0x38]
sub    rdx,QWORD PTR fs:0x28
jne    15c4
vzeroupper 
lea    rsp,[rbp-0x28]
pop    rbx
pop    r12
pop    r13
pop    r14
pop    r15
pop    rbp
ret    

~~~

</details>
</br>


**Hash table searching testing, AVX2 + inline assembler optimization, iteration count 500000000, searched word ="7lenstr"**  
![inline_asm_opt_time](pictures/inline_asm_opt_time.png)

**Hash table searching testing, AVX2 + inline assembler + static assembler optimization, iteration count 500000000, searched wrod="7lenstr"**
![asm_opt_time](pictures/asm_time.png)

As you can say it add nothing or even do worse in performance case. The reason mostly because the compiler can not, for example, inline this code. For compiler this code harder to optimize, so it can slow the program. I decided to show this optimization for study purposes only. 

### Conclusion <a name="opt_results"></a>
Thanks to optimization, it is possible to achieve acceleration, usually somewhere in 2 times.  
[ded32](https://github.com/ded32) performance coefficient:  

<img src="https://render.githubusercontent.com/render/math?math=\xi = \frac{\text{performance boost}}{\text{assembly lines}}\cdot{1000} \approx \frac{2}{155}\cdot 1000 \approx 12.9">
