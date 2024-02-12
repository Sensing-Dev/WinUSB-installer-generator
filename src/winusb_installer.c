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
                
                // Note that compoite device has its parent and this will generate and apply WinUSB driver on parent
                // Composite Parents returns is_composite false while its componends (each interface) returns true.
                if ( !device->is_composite ){
                    printf("Detect composite parent of USB device \"%s\" (%04X:%04X)\n", device->desc, device->vid, device->pid);
                    // Assume that all U3V Camera devices is composite device and has composite parent
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
                    printf("Detect composite device \"%s\" (%04X:%04X) but not composite parent... skip\n", device->desc, device->vid, device->pid);
                }
            }else{
                // printf("\n");
            }
        }

        wdi_destroy_list(list);
    }else{
        printf("Failed\n");
        return 1;
    }
    return 0;
}
