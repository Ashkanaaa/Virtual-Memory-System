#include "vm.h"
//FINAL3
typedef struct vm_info{
    size_t Ph_pages;
    FILE *F_swap;
    size_t Sw_pages;
    int active_p;
   
    int freePage;
    
}vm_info;


/////////////////////////////////////////


uint32_t extract_digits(int lower_idx, int upper_idx,uint32_t vaddr) {
    uint32_t mask = ((1 << (upper_idx - lower_idx + 1)) - 1) << lower_idx;
    uint32_t result = (vaddr & mask) >> lower_idx;
    return result;
}

uint32_t change_bit(uint32_t number, int n, int value) {
    uint32_t mask = 1 << n;  // create a mask with the nth bit set to 1
    if (value == 0) {
        number &= ~mask;  // set the nth bit to 0
    } else {
        number |= mask;  // set the nth bit to 1
    }
    return number;
}

// ****************************************************************************************
void Build_freeList(void *phys_mem){

    vm_info * ptr = (vm_info*) phys_mem;

    for(int x = 1;x<ptr->Ph_pages;x++){
        uint32_t* val = (uint32_t*) (phys_mem + x * 4096);
        if(x == (ptr->Ph_pages-1)){
            *val = 0;

        }else{
            *val = (x+1);
        }
    }    
}


void Init_Page(void *phys_mem){
    vm_info * ptr = (vm_info*) phys_mem;

    for(int x = 1;x<ptr->Ph_pages;x++){
        for(int y = 0;y<4096;y+=4){
            uint32_t* val = (uint32_t*) (phys_mem + x * 4096+y);
            *val = 0;
    
        }
    }    
}

//*************************************************************
void Unmap_page(void*vm,uint32_t page,bool firstpage){
    vm_info * info = (vm_info*)vm;
    void *ptr;
    bool valid = false;
    for(int x = 0;x<4096;x+=4){
        ptr = vm + page *4096 + x;
        if(extract_digits(0,0,*((uint32_t*)ptr)) == 1){
            valid = true;
            break;
        }
    }
    if(valid == false && firstpage == true){
        ptr = vm + page *4096;
        *((uint32_t*)ptr) = info->freePage;
        info->freePage = page;
        info->active_p = (info->active_p) - 1;
    }
    else if(valid == false){
        ptr = vm + page *4096;
        *((uint32_t*)ptr) = info->freePage;
        info->freePage = page;
    }

}

// int Find_freeIndex(void *phys_mem){
    
//     vm_info * ptr = (vm_info*) phys_mem;
//     for(int x = 0;x<1000;x++){
//         if(ptr->used_addr[x] <0){
//             return x;
//         }
//     }
//     return -1;
// }

// bool Mapping_exists(void * vm,vaddr_t addr){
//     vm_info * ptr = (vm_info*) vm;
//     for(int x=0;x<1000;x++){
//         if(ptr->used_addr[x] == addr){
//             return true;
//         }
//     }

//     return false;
// }


//finds a page and returns the page number
//returns 0 if no page found
bool num_freePage(void * vm,int num){
    vm_info * info = (vm_info*) vm;
    if(info->freePage == 0){
        return false;
    }
    //going to the first free page if it exists
    void * ptr = vm + info->freePage * 4096;
    int count = 1;
    
    while(*((uint32_t *)(ptr))>0){
        count++;
        ptr = vm + *((uint32_t *)(ptr)) * 4096;
    }
    //printf("freepage:%d\n",count);

    if(num <=count){
        return true;
    }else{
        return false;
    }
}

uint32_t find_Fpage(void *vm){
    vm_info * info = (vm_info*) vm;
    //check to see if there is any page available
    if(info->freePage == 0){
        return 0;
    }else{
        uint32_t page = info->freePage;
        void * ptr = vm + info->freePage * 4096;
        info->freePage = *((uint32_t *)(ptr));
       *((uint32_t *)(ptr)) = 0;
        return page;

    }
    //     bool found =false;//to see if there is any other page free except the one that was initially free

    //     void * ptr = vm + info->freePage * 4096;
    //     while(*((uint32_t *)(ptr))!=0){  
    //         if(page != *((uint32_t *)(ptr))){
    //             found = true;
    //             info->freePage = *((uint32_t *)(ptr));
    //             break;
    //         }
    //         ptr = vm + *((uint32_t *)(ptr)) * 4096;
    //     }

    //     //set the header of the returned page to 0
    //     ptr = vm + page * 4096;
    //     *((uint32_t *)(ptr)) = 0;

    //     if(found){
    //         return page;
    //     }else{
    //         info->freePage = 0;
    //         return page;
    //     }


    
    /////////////////////////////////////////////////////////////////
    // if(First_time){
    //     for(int x = 1;x< ptr->Ph_pages;x++){
    //         if(extract_digits(7,7,*((uint32_t *)(vm + x * 4096))) == 0){
    //             available++;
    //         }
    //     }
    //     //printf("%d || %d\n",ptr->Ph_pages,available);
    //     if(available <3)return 0;
    // }
    
    
    // while(extract_digits(7,7,*((int *)(vm + freePage * 4096))) == 1){
    //     freePage++;
    //     found = true;
    // }

    // if(freePage == 1){
    //     uint32_t PTE = *((uint32_t *)(vm + freePage * 4096));
    //     *((uint32_t *)(vm + freePage * 4096))= change_bit(PTE,7,1);
    //     //printf("page found %u\n", *((uint32_t *)(vm + freePage * 4096)));
    //     return 1;
    // }

    // if(found){
    //     uint32_t PTE = *((uint32_t *)(vm + freePage * 4096));
    //     *((uint32_t *)(vm + freePage * 4096))= change_bit(PTE,7,1);
        
    //     return freePage;
    // }else{
    //     printf("last\n");
    //     return 0;
    // }
}


void *vm_init(void *phys_mem, size_t num_phys_pages, FILE *swap, size_t num_swap_pages) {
    // TODO
    vm_info *info_ptr = (vm_info*) phys_mem;
    info_ptr->Ph_pages = num_phys_pages;
    info_ptr->F_swap = swap;
    info_ptr->Sw_pages = num_swap_pages; 
    info_ptr->active_p = 0;
    info_ptr->freePage = 1;

    Init_Page(phys_mem);
    Build_freeList(phys_mem);
    
    return phys_mem;         


}

void vm_deinit(void *vm) {
    // TODO
    Init_Page(vm);
}

vm_result_t vm_map_page(void *vm, bool new_process, paddr_t pt, vaddr_t addr,
                        bool user, bool exec, bool write, bool read) {
        // TODO
        vm_info * info = (vm_info*) vm;
        void * ptr;
        vm_result_t result;

        if(info->active_p ==1000 && new_process == true){
            result.status = VM_OUT_OF_MEM;
            //printf("first\n");
            return result;
        }
        // else if(Mapping_exists(vm,addr)){
        //     result.status = VM_DUPLICATE;
        //     return result;
        // }
    

    uint32_t access = 0;
    access = change_bit(access,0,1);
    access = change_bit(access,1,1);
    if(user){access = change_bit(access,5,1);}
    if(exec){access =change_bit(access,4,1);}
    if(write){access =change_bit(access,3,1);}
    if(read){access =change_bit(access,2,1);}
  
    if(new_process){

        bool enough = num_freePage(vm,3);
        if(enough == false){
            result.status = VM_OUT_OF_MEM;
            return result;
        }
        uint32_t first = find_Fpage(vm);
        if(first == 0){
            //printf("first\n");
        }
        
        uint32_t second = find_Fpage(vm);
        if(second == 0){
            //printf("second\n");
        }
        
        uint32_t third = find_Fpage(vm);
        if(third == 0){
            //printf("third\n");
        }

        //if no page is available
        if(first ==0 || second == 0 || third == 0 || enough == false){
            result.status = VM_OUT_OF_MEM;
            //printf("didnt catch the error\n");
            return result;
        }

        //initializing first PTE
        ptr = vm+ first*4096 + extract_digits(22,31,addr)*4;
        //if the entry was already mapped
        if(extract_digits(0,0,*((uint32_t*)ptr)) == 1){
            result.status = VM_DUPLICATE;
            return result;
        }
        //printf("AAAAAAAA %u BBB %u\n",change_bit(second * 4096 + access,8,1),second);
        *((uint32_t*)ptr) = change_bit(second * 4096 + access,8,1);
        
        
        //initializing second PTE
        ptr = vm+ second*4096 + extract_digits(12,21,addr)*4;
        //if entry was already mapped
        if(extract_digits(0,0,*((uint32_t*)ptr)) == 1){
            result.status = VM_DUPLICATE;
            return result;
        }
        *((uint32_t*)ptr) = change_bit(third * 4096 + access,8,1);

        //adding the virtual adderres to the list
        // int index = Find_freeIndex(vm);
        // if(index < 0){
        //     result.status = VM_OUT_OF_MEM;
        //     return result;
        // }
        
        info->active_p = (info->active_p+1);

        //returning the resulting addr
        result.addr = (paddr_t) first * 4096;
        result.status = VM_OK;
        return result;
    

    }else{
        
        //getting to the first PTE
        ptr = vm + pt + extract_digits(22,31,addr)*4;
        uint32_t PTE1 = *((uint32_t*) ptr);
    

        //if the PTE was already mapped
        if(extract_digits(0,0,PTE1) == 1){
            //bringing the pointer to the second PTE
            //printf("already mapped\n");
            uint32_t second = extract_digits(12,31,PTE1);
            ptr = vm +second * 4096 + extract_digits(12,21,addr)*4; 


            //check to see PTE2 is not already mappped
            if(extract_digits(0,0,*((uint32_t*)ptr)) == 1){
                result.status = VM_DUPLICATE;
                //printf("Second PTE mapped as well\n");
                return result;
            }

            //if a page is available
            if(num_freePage(vm,1)){
                uint32_t third = find_Fpage(vm);
                if(third == 0){
                    result.status = VM_OUT_OF_MEM;
                    //printf("didnt catch the error\n");
                    return result;
                }
                *((uint32_t*)ptr) = change_bit(third * 4096 + access,8,1);
                
                 //add to the array
                // int index = Find_freeIndex(vm);
                // if(index < 0){
                //     result.status = VM_OUT_OF_MEM;
                //     return result;
                // }
                 
               


                result.addr = pt;
                result.status = VM_OK;
                return result;
            }else{
                result.status = VM_OUT_OF_MEM;
                return result;
            }
            
            
          //if a PTE was not already mapped
        }else{
           //if two pages is available 
           if(num_freePage(vm,2)){

                //mapping the first PTE
                uint32_t second = find_Fpage(vm);
                if(second == 0){
                    result.status = VM_OUT_OF_MEM;
                    //printf("didnt catch the error\n");
                    return result;
                }
                
                *((uint32_t*) ptr) = change_bit(second * 4096 + access,8,1);
               
                //mapping the second PTE
                ptr = vm + second * 4096 + extract_digits(12,21,addr)*4; 
                uint32_t third = find_Fpage(vm);
                if(third == 0){
                    result.status = VM_OUT_OF_MEM;
                    //printf("didnt catch the error\n");
                    return result;
                }
                
                *((uint32_t*)ptr) = change_bit(third * 4096 + access,8,1);
                 
                 //add to the array
                // int index = Find_freeIndex(vm);
                // if(index < 0){
                //     result.status = VM_OUT_OF_MEM;
                //     return result;
                // }
                

                result.addr = pt;
                result.status = VM_OK;
                return result;
            }else{
                
                result.status = VM_OUT_OF_MEM;
                return result;
            }   
        }

    }
    


}

vm_status_t vm_unmap_page(void *vm, paddr_t pt, vaddr_t addr) {
    // TODO
    vm_status_t stat;
    vm_info * info = (vm_info*) vm;
    //going to the first PTE
    void * ptr = vm + pt + extract_digits(22,31,addr)*4;
    
    //check to see if there is mapping in the first level page
    if(extract_digits(0,0,*((uint32_t*)ptr)) !=1){
        stat = VM_BAD_ADDR;
        return stat;
    }
    
    uint32_t page2 = extract_digits(12,31,*((uint32_t*)ptr));
    *((uint32_t*)ptr) = 0;//freeing the PTE1
    Unmap_page(vm,(pt/4096),true);

    //going to the second page table 
    ptr = vm +  page2* 4096 + extract_digits(12,21,addr)*4;
    if(extract_digits(0,0,*((uint32_t*)ptr)) !=1){
        stat = VM_BAD_ADDR;
        return stat;
    }
    uint32_t page3 = extract_digits(12,31,*((uint32_t*)ptr));
    *((uint32_t*)ptr) = 0;//freeing the PTE2
    Unmap_page(vm,page2,false);

    //going to the third page and adding it to the freelist
    ptr = vm +  page3* 4096;
    *((uint32_t*)ptr) = info->freePage;
    info->freePage = page3;

    //if everything was ok return VM_Ok
    // printf("pointer:%d\n",info->freePage);
    
    stat = VM_OK;
    return stat;

    
}

vm_result_t vm_translate(void *vm, paddr_t pt, vaddr_t addr, access_type_t acc, bool user) {
    // 
    vm_info* casted_vm = (vm_info *) vm;
    vm_result_t results;
    //setting the access type
    bool exec = true;
    if(acc > 0){
        exec = false;
        acc++;
    }


    //getting to the first PTE
    void* A_P1 = vm + pt + extract_digits(22,31,addr)*4;
    uint32_t PTE1 = change_bit(*((uint32_t *) A_P1),6,1);
    *((uint32_t *) A_P1) = PTE1;
    
    //check to see if next page is presented and valid
    if(extract_digits(0,0,PTE1) !=1 ||extract_digits(1,1,PTE1) !=1){
        //printf("here");
        results.status = VM_BAD_ADDR;
        return results;
    }

    //getting to the second PTE
    void* A_P2 = vm + extract_digits(12,31,PTE1) * 4096 + extract_digits(12,21,addr)*4;
    uint32_t PTE2 = change_bit(*((uint32_t *) A_P2),6,1);
    *((uint32_t *) A_P2) = PTE2;

    //check to see if next page is presented and valid && check for permissions
    if(extract_digits(0,0,PTE2) == 0 ||extract_digits(1,1,PTE2)==0){
        //printf("here %u\n",PTE2);
        results.status = VM_BAD_ADDR;
        return results;
    }else if (exec){
        if(extract_digits(4,4,PTE2) != 1){
            results.status =VM_BAD_PERM;
            return results;
        }
    }else if(extract_digits(acc,acc,PTE2) != 1){
            results.status =VM_BAD_PERM;
            return results;
    }
        
    
    //check for user permission
    if(user == true && extract_digits(5,5,PTE2) != 1){
            results.status =VM_BAD_PERM;
            return results; 
    }

    // vm_info* A_P3 =  casted_vm + extract_digits(12,31,PTE2) * 4096 + extract_digits(0,11,addr);
    // results.addr = *((paddr_t*) A_P3);
    //printf("PTE2:%u\n",PTE2);
    results.addr = (paddr_t) extract_digits(12,31,PTE2) * 4096 + extract_digits(0,11,addr);
    results.status = VM_OK;

    return results;

    

}

void vm_reset_accessed(void *vm, paddr_t pt) {
    // TODO

}

//FINAL10
