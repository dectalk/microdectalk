
# DECtalk Dictionary Format 

The DECtalk dictionary format starts with 8 bytes. These bytes are used as follows: 
1. **First 4 Bytes**: Represent the **entry count** in the dictionary.
2. **Next 4 Bytes**: Represent the **byte count** of the data section (excluding the index array).

The `get_long_int` macro is used to extract these values:
```C
#define get_long_int(ptr) ((U32)\
                       ((((U8 *)(ptr))[3] << 24)  | \
                        (((U8 *)(ptr))[2] << 16)  | \
                        (((U8 *)(ptr))[1] << 8)  | \
                        (((U8 *)(ptr))[0])))
```
(keep note of this macro for later)

example usage to get entry count:
```C
int entries = get_long_int((char *)dictionary);
```
and to get the byte count:
```C
int bytes = get_long_int((char *) &dictionary[4]);
```
We use `get_long_int()` for both of these operations because both entries and bytes are stored as 4-byte long integers. The entry count starts at byte 0, and the byte count starts at byte 4, which is why we use an offset of `+4` to get the byte count. When working with the data section, we use a `+8` offset to account for both the entry count and byte count.

### The Offset Array

After the 8-byte header, the next section is the **offset array**, which helps locate the positions of individual entries in the dictionary data.

The length of the offset array is calculated as:
```C
(entries * 4) + 8
```
**To get the location of the dictionary data**, you can use the following line:
```C
char *data = &dictionary[(entries * 4) + 8];
```
The **offset array** starts at byte 8, after the entry count and byte count:
```C
char *offsets = &dictionary[8];
```

### Dictionary Data Format

Each entry consists of:

-   An **ASCII word** (terminated by a null byte).
-   **Phoneme data** follows the word and is also terminated by a null byte.

Here's a phoneme map for decoding:
```C
#define SIL              0              /* Must be zero                 */
#define IY               1
#define IH               2
#define EY               3
#define EH               4
#define AE               5
#define AA               6
#define AY               7
#define AW               8
#define AH               9
#define AO              10
#define OW              11
#define OY              12
#define UH              13
#define UW              14
#define RR              15
#define YU              16
#define AX              17
#define IX              18
#define IR              19
#define ER              20
#define AR              21
#define OR              22
#define UR              23
#define W               24
#define Y               25
#define R               26
#define LL              27
#define HX              28
#define RX              29
#define LX              30
#define M               31
#define N               32
#define NX              33
#define EL              34
#define DZ    35
#define EN              36
#define F               37
#define V               38
#define TH              39
#define DH              40
#define S               41
#define Z               42
#define SH              43
#define ZH              44
#define P               45
#define B               46
#define T               47
#define D               48
#define K               49
#define G               50
#define DX              51
#define TX              52
#define Q               53              /* glottal stop                 */
#define CH              54
#define JH              55
#define DF              56
```
These phonemes are stored as non-ASCII integers. The map above corresponds to the speech sounds used by DECtalk. This list does not include command phonemes (e.g., for stress or prosody).

### Extracting a Specific Entry

To access a specific dictionary entry, first calculate the offset from the **offset array** and use it to retrieve the entry from the **dictionary data**.

Since the entry consists of the word followed by phonemes, we add `+4` to the index to skip the word and its null terminator.
```C
int ind = get_long_int(&offsets[index * 4]);
char *entry = &data[ ind + 4 ];`
```

finally, if you want to see an example parser, just check out [parse.c](https://github.com/dectalk/microdectalk/docs/parse.c)
i apologize for the messy parser code lol
