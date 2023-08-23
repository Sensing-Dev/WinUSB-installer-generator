#include <stdio.h>
#include<stdbool.h>
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

    if (argc != 2){
        usage("missing argument for parameter");
    }

    vendor_id = (int)strtol(argv[1], NULL, 16);
    printf("%i\n", vendor_id); 

    // list all
    struct wdi_options_create_list option = {TRUE, TRUE, TRUE};

    if (wdi_create_list(&list, &option) == WDI_SUCCESS) {
        for (device = list; device != NULL; device = device->next) {
            printf("Installing driver for USB device: \"%s\" (%04X:%04X)",
                device->desc, device->vid, device->pid);
            if (device->vid == vendor_id){
                printf(" <-- these are the target\n");

                struct wdi_options_prepare_driver driver_option ={
                    WDI_WINUSB, 
                    wdi_get_vendor_name(device->vid)
                };

                if (wdi_prepare_driver(device, ".", "target_device.inf", &driver_option) == WDI_SUCCESS) {
                    wdi_install_driver(device, ".", "target_device.inf", NULL);
                }
                return 0;
            }else{
                printf("\n");
            }

        }

        wdi_destroy_list(list);
    }else{
        printf("Faile\n");
    }

    return 0;
}
