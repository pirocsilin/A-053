// intrfECE0206.h
//
// Generated by C DriverWizard 3.2.0 (Build 2485)
// Requires DDK Only
// File created on 4/6/2013
//

// Define an Interface Guid for ece0206_1 device class.
// This GUID is used to register (IoRegisterDeviceInterface)
// an instance of an interface so that user application
// can control the ece0206_1 device.
//
//  {2029511E-8F33-4910-87BA-595A48CACF63}


//! DEFINE_GUID(GUID_DEVINTERFACE_ECE0206_1,
//! 0xc32079a6, 0x2663, 0x4647, 0xb0, 0xa4, 0xa2, 0xc2, 0x4, 0xa9, 0x34, 0x1);


 //   0x2029511E, 0x8F33, 0x4910, 0x87, 0xBA, 0x59, 0x5A, 0x48, 0xCA, 0xCF, 0x63);
// {C32079A6-2663-4647-B0A4-A2C204A93401}
//IMPLEMENT_OLECREATE(<<class>>, <<external_name>>, 
//0xc32079a6, 0x2663, 0x4647, 0xb0, 0xa4, 0xa2, 0xc2, 0x4, 0xa9, 0x34, 0x1);
//static const GUID <<name>> = 
//{ 0xc32079a6, 0x2663, 0x4647, { 0xb0, 0xa4, 0xa2, 0xc2, 0x4, 0xa9, 0x34, 0x1 } };
//{C32079A6-2663-4647-B0A4-A2C204A93401}

// Define a Setup Class GUID for ece0206_1 Class. This is same
// as the ECE0206_1 CLASS guid in the INF files.
//
//  {8078A2D3-663A-48D1-A4E3-0DBA867DBCBE}


//!DEFINE_GUID(GUID_DEVCLASS_ECE0206_1,
//    0x8078A2D3, 0x663A, 0x48D1, 0xA4, 0xE3, 0x0D, 0xBA, 0x86, 0x7D, 0xBC, 0xBE);
//!      0xb28e9f78, 0xa66c, 0x44e7, 0x84, 0x88, 0xad, 0x29, 0x3, 0xb1, 0x42, 0xb4 );


// GUID definition are required to be outside of header inclusion pragma to avoid
// error during precompiled headers.
//
