import os 
import sys

super_block = []
group = []
inodes = []
blocks = []
free_blocks = []
free_inodes = []
indirects = []
directs = []
reserved_blocks = {1, 2, 3, 4, 5, 6, 7, 64}
allocated_blocks = set()
allocated_inodes = set()
links = {}
link_counter = {}
duplicate_blocks = {}

ok_file = 1

if len(sys.argv) != 2:
    os.write(2, "invalid arguments\n")
    exit(1)

#f = 0
try:
    f = open(sys.argv[1], 'r')
except IOError:
    os.write(2, "file error\n")
    exit(1)

current_line = f.readline()
#max_blocks = int(group[2])

while current_line:
    current_data = current_line.replace("\n", "").split(',')
    current_line  = f.readline()
    if(current_data[0] == "SUPERBLOCK"):
        super_block = current_data
    elif(current_data[0] == "GROUP"):
        group = current_data
    elif(current_data[0] == "BFREE"):
        free_blocks.append(int(current_data[1]))
    elif(current_data[0] == "IFREE"):
        free_inodes.append(int(current_data[1]))     
    elif(current_data[0] == "DIRENT"): 
        directs.append(current_data)
    elif(current_data[0] == "INDIRECT"): 
       indirects.append(current_data)
    elif(current_data[0] == "INODE"):
        inodes.append(current_data)

max_blocks = int(group[2])

for inode in inodes:
    links[int(inode[1])] = int(inode[6])
    for i in range(12, 27):
        type_str = " "
        offset = 0
        if(i == 24):
            type_str = " INDIRECT "
            offset = 12
        elif(i == 25):
            type_str = " DOUBLE INDIRECT "
            offset = 256 + 12
        elif(i == 26):
            type_str = " TRIPLE INDIRECT "
            offset = (256 * 256) + 256 + 12
        if(int(inode[i]) < 0 or int(inode[i]) > max_blocks):
            print("INVALID" + type_str + "BLOCK " +  inode[i] + " IN INODE " + inode[1] + " AT OFFSET " + str(offset))
            ok_file = 0
        elif(int(inode[i]) != 0 and int(inode[i]) in reserved_blocks):
            print("RESERVED" + type_str + "BLOCK " +  inode[i] + " IN INODE " + inode[1] + " AT OFFSET " + str(offset))
            ok_file = 0
        else:
            allocated_blocks.add(int(inode[i]))
            if int(inode[i]) not in duplicate_blocks:
                duplicate_blocks[int(inode[i])] = [[type_str, int(inode[i]), int(inode[1]), str(offset)]]
            else:
                duplicate_blocks[int(inode[i])].append([type_str, int(inode[i]), int(inode[1]), str(offset)])

    allocated_inodes.add(int(inode[1]))

for indirect in indirects:
    i = int(indirect[2])
    type_str = " "
    offset = 0
    if(i == 1):
        type_str = " INDIRECT "
        offset = 12
    elif(i == 2):
        type_str = " DOUBLE INDIRECT "
        offset = 256 + 12
    elif(i == 3):
        type_str = " TRIPLE INDIRECT "
        offset = (256 * 256) + 256 + 12
    if(int(indirect[5]) < 0 or int(indirect[5]) > max_blocks):
        print("INVALID" + type_str + "BLOCK " +  indirect[5] + " IN INODE " + indirect[1] + " AT OFFSET " + str(offset))
        ok_file = 0
    elif(int(indirect[5]) != 0 and int(indirect[5]) in reserved_blocks):
        print("RESERVED" + type_str + "BLOCK " +  indirect[5] + " IN INODE " + indirect[1] + " AT OFFSET " + str(offset))
        ok_file = 0
    else:
        allocated_blocks.add(int(indirect[5]))
        if int(indirect[5]) not in duplicate_blocks:
            duplicate_blocks[int(indirect[5])] = [[type_str, int(indirect[5]), int(indirect[1]), str(offset)]]
        else:
            duplicate_blocks[int(indirect[5])].append([type_str, int(indirect[5]), int(indirect[1]), str(offset)])

for i in range(8, max_blocks):
    if(i not in free_blocks and i not in allocated_blocks):
        print("UNREFERENCED BLOCK " + str(i))
        ok_file = 0
    elif(i in free_blocks and i in allocated_blocks):
        print("ALLOCATED BLOCK " + str(i) + " ON FREELIST")
        ok_file = 0

for key in duplicate_blocks:
    if key != 0 and len(duplicate_blocks[key]) != 1: #error
        for block in duplicate_blocks[key]:
            print("DUPLICATE" + block[0] + "BLOCK " + str(block[1]) + " IN INODE " + str(block[2]) + " AT OFFSET " + str(block[3]))
            ok_file = 0

for i in range(int(super_block[7]), int(group[3]) + 1):
    if i not in free_inodes and i not in allocated_inodes:
        print("UNALLOCATED INODE " + str(i) + " NOT ON FREELIST")
        ok_file = 0

for i in free_inodes:
    if i in allocated_inodes:
        print("ALLOCATED INODE " + str(i) + " ON FREELIST")
        ok_file = 0

for direct in directs:
    if(int(direct[3]) not in link_counter):
        link_counter[int(direct[3])] = 1
    else:
        link_counter[int(direct[3])] += 1

#for inode in inodes:
    #cases -> the inodes count is zero, link_counter is not 0 
    #the inodes counter is not zero and is not in link_counter
    #the inodes counter != link_counter

    #if(int(inode[6]) != link_counter[int(inode[1])])
#print(link_counter)
for inode in inodes:
    #######3
    error = 0
    link_count = 0
    if int(inode[6]) == 0 and int(inode[1]) in link_counter:
        error = 1
        link_count = link_counter[int(inode[1])]
    elif int(inode[6]) != 0 and int(inode[1]) not in link_counter:
        error = 1
    elif int(inode[6]) != link_counter[int(inode[1])]:
        error = 1
        link_count = link_counter[int(inode[1])]

    if error == 1:
        print("INODE " + inode[1] + " HAS " + str(link_count) + " LINKS BUT LINKCOUNT IS " + inode[6])
        ok_file = 0
    ################
    



#print(free_inodes)
directory_map = {}
directory_map[2] = 2
for direct in directs:
    if direct[6] != "'.'" and direct[6] != "'..'": 
        directory_map[int(direct[3])] = int(direct[1])
    if int(direct[3]) < 1 or int(direct[3]) > int(group[3]):
        print("DIRECTORY INODE " + direct[1] + " NAME " + direct[6] + " INVALID INODE " + direct[3])
        ok_file = 0
    elif int(direct[3]) in free_inodes and int(direct[3]) not in allocated_inodes:
        print("DIRECTORY INODE " + direct[1] + " NAME " + direct[6] + " UNALLOCATED INODE " + direct[3])
        ok_file = 0
    
for direct in directs:
    if direct[6] == "'.'" and direct[3] != direct[1]: #direct[6] != "'..'": #map each entry to its parent
        print("DIRECTORY INODE " + direct[1] + " NAME '.' LINK TO INODE " + direct[3] + " SHOULD BE " + direct[1])
        ok_file = 0
    elif direct[6] == "'..'" and int(direct[3]) != directory_map[int(direct[1])]:
        print("DIRECTORY INODE " + direct[1] + " NAME '..' LINK TO INODE " + direct[3] + " SHOULD BE " + str(directory_map[int(direct[1])]))
        ok_file = 0

if ok_file == 0:
    exit(0)
else:
    exit(2)