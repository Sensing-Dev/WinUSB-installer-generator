#include <stdio.h>
#include <stdbool.h>
#include <libwdi.h>

#include <stdio.h>
#include <string.h>

void usage(const char* msg){
    printf("%s\n", msg);
    printf("usage:\n");
    printf("./winusb-installer.exe <vender id>");
}


int main(int argc, char* argv[]){
    struct wdi_device_info *device, *list;

    bool find_target_device = false;
    int vendor_id = 0;
    int product_id = 0;

    if (argc != 3){
        printf( "missing argument for parameter" ); 
        return 1;
    }

    vendor_id = (int)strtol(argv[1], NULL, 16);
    product_id = (int)strtol(argv[2], NULL, 16);
    printf("Targeted vernder id is %s and product id is %s\n", argv[1], argv[2]); 

    // list all
    // https://github.com/pbatard/libwdi/wiki/Usage#struct-wdi_options_create_list
    struct wdi_options_create_list option = {TRUE, TRUE, TRUE};

    if (wdi_create_list(&list, &option) == WDI_SUCCESS) {
        for (device = list; device != NULL; device = device->next) {
            if (device->vid == vendor_id && device->pid == product_id){
                printf("Installing driver on USB device: \"%s\" (%04X:%04X)",
                    device->desc, device->vid, device->pid );
                    if (device->is_composite){
                        printf("COMPOSITE %u\n", device->mi);
                    }else{
                        printf("\n");
                    }
                
            // if (device->vid == vendor_id && device->pid == product_id){

                struct wdi_options_prepare_driver driver_option ={
                    WDI_WINUSB, 
                    wdi_get_vendor_name(device->vid)
                };

                if (wdi_prepare_driver(device, ".", "target_device.inf", &driver_option) == WDI_SUCCESS) {
                    printf("********************************************************\n");
                    printf("* This process may take up to 5 minutes... please wait *\n");
                    printf("********************************************************\n");
                    wdi_install_driver(device, ".", "target_device.inf", NULL);
                }
            }else{
                // printf("\n");
            }
        }

        wdi_destroy_list(list);
    }else{
        printf("Failed\n");
    }
    return 0;
}
