# 1.8.2 - Cross Platform For Mac And IOS

Currently the Windows build for android does not render any items.  Currently the Mac build for android does not render GUI items.

When using OnRender, you now have to pass your logic into a callback function as follows:

    pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
        &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourAndroidMobileBluetoothCallback, &yourMacIosMobileBluetoothCallback);

This function should be in the OnCreate function of a Pen Engine instantiation, for example:

    class App : public pen::Pen {
    public:
        void OnCreate() override {
            pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
                &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourAndroidMobileBluetoothCallback, &yourMacIosMobileBluetoothCallback
                &yourMobileHttpCallback);
            pen::Pen::SetMobileTextures(pathListVector);

            /*All logic in OnCreate should come after the two previous functions*/
        }
    }

pen::Pen::SetMobileTextures is only used for gui sprite sheets, if loading anything else do:

    pen::Asset asset = pen::Asset::Load(std::string file, char*(*onLoad)(const char* path, long* fileLength) = nullptr);

Check out section 1.1.1 for more on managing assets.
If specifically loading in images to be rendered then you can take the asset data and add it to the sprite map by doing:

    pen::State::Get()->pixelSprites.Insert(asset.name, pen::Sprite{asset.name, (unsigned char*)asset.data, 0, 0});
    
Pen Engine looks for this instantiation in app.h, but you can change the name of that file include in 
pen_engine/ext/platforms/mac_ios/mac_ios_view_delegate.h.

There is a difference for how you would do the mobile render callback function for Mac and IOS though, notice instead of
a while loop for PC, you do an if statement:

    void yourMacIOSRenderFunction(){
        if (pen::Pen::Running())
        {
            if(pen::Render::Get()->firstTime) pen::Render::Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender();
        }
    }

Also input would have to be done on the native side for Android while implementing one of the mobile callback functions.

The PC function would remain the same with the while loop:

    void OnRender() override {
        while (pen::Pen::Running())
        {
            OnInput();
            if(pen::Render::Get()->firstTime) pen::Render::Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender();
        }
    }

To get started building for Mac and IOS, first go to pen_engine/state/config.h and uncomment this line:

    //#define __PEN_MAC_IOS__

If you uncomment this line, your application will not run for PC, so only do this once you have tested
on the PC version and are ready to build it for Mac and IOS.

For using Xcode, first go and create a target inside of your ios application if you have not done so already.

Next go to Build Phases -> Link Binary With Libraries -> add the following libraries:

- Foundation
- Metal
- QuartzCore
- MetalKit
- CoreBluetooth

For IOS:

- UIKit

For Mac:

- AppKit

After that, right click on your target in the project explorer and add pen_engine to your target, this should
automatically populate Build Phases -> Compile Sources.

In Build Phases -> Copy Bundle Resources make sure to remove any android resources in there that may have
been copied.  If any errors are encountered from the copy bundle section then remove those files as well.

In Build Settings -> C++ Language Dialect, make sure it is C++17.

---------------------------------------------------------------------------

# 1.8.2.1 - Mac And IOS Bluetooth

# Bluetooth As A Central

If using bluetooth you have to add a description for the bluetooth permission in
your Info.plist file.

For connecting to other devices via bluetooth, first search for available devices:

    pen::ios::conn::bt::Scan(std::vector<std::string> devices = {});

The devices' names do not need to be known before searching, it will just find all of the available devices.

While scanning, potential connections can be constantly queried with:

    std::vector<std::string> devices = pen::ios::conn::bt::QueryPotentialConnections();
    
Once you have a desired device that you want to connect with, you connect by doing:

    pen::ios::conn::bt::Connect(std::string device, std::string descriptor);

This descriptor is the same name as the characteristic associated with the service you want to interact
with for the given device, this should be known beforehand when connecting.

You can connect with multiple devices at once.

Once a connection is established a subscription is made to automatically read data from the device
within the bluetoothCallback, data can be read by doing:

    void macIosBluetoothCallback(char* bytes, long length, unsigned int type){
        /*Handle the value from the characteristic*/
        if(type == pen::ios::conn::bt::TYPE::CENTRAL){
        
        }
    }
    
You can also read data manually by doing with the callback being the same:

    pen::ios::conn::bt::Read(const char* device);

    void macIosBluetoothCallback(char* bytes, long length, unsigned int type){
        /*Handle the value from the characteristic*/
        if(type == pen::ios::conn::bt::TYPE::CENTRAL){
        
        }
    }

Data can written to the current characteristic for all connected devices by doing:

    pen::ios::conn::bt::Write(char* data, long length);

Once done with a connected device, the connection can be closed by doing:

    pen::ios::conn::bt::Disconnect(const char* device);
    
# Bluetooth As A Peripheral

When using bluetooth as a peripheral device you initiate bluetooth by doing:

    pen::ios::conn::bt::rec::AddService(const char* service, const char* characteristic, char* value, long length, unsigned int type);

The types can be defined as the following:

    - pen::ios::conn::bt::TYPE::READ for a readable service characteristic
    - pen::ios::conn::bt::TYPE::READ for a writable service characteristic
    
Now your service will be advertised to any centrals that are available to connect.

If a central subscribes to your service, then you can update the value of the characteristic by doing:

    void macIosBluetoothCallback(char* bytes, long length, unsigned int type){
        /*Update the value of the characteristic*/
        if(type == pen::ios::conn::bt::TYPE::PERIPHERAL){
            /*
            The bytes variable is now a string that has the name of the service and characteristic
            separated by a colon
            */
            std::string serviceCharacteristicPair(bytes);
            std::string serviceName = serviceCharacteristicPair.substr(0, serviceCharacteristicPair.find(":"));
            std::string characteristicName = serviceCharacteristicPair.substr(serviceCharacteristicPair.find(":") + 1);
            
            /*The length is used to update the value with the correct number of bytes*/
            char* data = new char[length];
            
            /*Set the data*/

            pen::ios::conn::bt::rec::UpdateCharacteristic(serviceName, characteristicName, data);
            delete[] data;
        }
        
    }

---------------------------------------------------------------------------

# 1.8.2.2 - Mac And IOS HTTP

To do an http requests do the following:

    pen::ios::conn::http::Send(const std::string& url, unsigned int type, pen::Map<std::string,std::string>* httpBody = nullptr);
    
The types are defined as the following:

    - pen::ios::conn::http::TYPE::GET
    - pen::ios::conn::http::TYPE::POST
    
If expecting anything back from the request do:

    void your httpCallback(pen::Map<std::string,std::string> response){
        /*Handle the response*/
    }
}

---------------------------------------------------------------------------

# 1.8.2.3 - Mac And IOS Sockets

To do a socket request to a server first connect:

    pen::ios::conn::socket::Connect(const std::string& url);
    
Once connected you can be notified by doing:

    void your socketCallback(char* data, unsigned int messageType){
        /*Perform send or receive requests*/
        if(messageType == pen::ios::conn::socket::TYPE::CLOSED_WITH_ERROR){
            /*An error has occurred during the connection with the server
        }
    }

The message types are defined as the following:

    - pen::ios::conn::socket::TYPE::CONNECTED after the device has connected a server
    - pen::ios::conn::socket::TYPE::DATA_RECEIVED after the device has received data from a server
    - pen::ios::conn::socket::TYPE::CLOSED_WITH_ERROR if an error has occurred during a connection
    
When sending data to a server do:

    pen::ios::conn::socket::Send(char* data, long length);

When receiving data from a server do:

    pen::ios::conn::socket::Receive();
        
Once the data is received you can be notified by doing:

    void your socketCallback(char* data, unsigned int messageType){
        /*Handle received data*/
        if(messageType == pen::ios::conn::socket::TYPE::CLOSED_WITH_ERROR){
            /*An error has occurred during the connection with the server
        }else{
        
        }
    }

---------------------------------------------------------------------------