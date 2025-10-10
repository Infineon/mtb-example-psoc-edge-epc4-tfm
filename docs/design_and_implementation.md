[Click here](../README.md) to view the README.

## Design and implementation

The design of this application is minimalistic to get started with code examples on PSOC&trade; Edge MCU devices. All PSOC&trade; Edge E84 MCU applications have a dual-CPU three-project structure to develop code for the CM33 and CM55 cores. The CM33 core has two separate projects for the secure processing environment (SPE) and non-secure processing environment (NSPE).

This code example demonstrates how to use TrustedFirmware-M (TF-M) with Infineon's PSOC&trade; Edge MCU. TF-M implements the SPE for Armv8-M and Armv8.1-M architectures (e.g., the Cortex&reg;-M33, Cortex&reg;-M23, Cortex&reg;-M55, and Cortex&reg;-M85 processors) and dual core platforms. It is platform security architecture reference implementation aligning with PSA-certified guidelines, enabling chips, real-time operating systems, and devices to become PSA-certified. For more details, see the [TrustedFirmware-M documentation](https://tf-m-user-guide.trustedfirmware.org/).

The extended boot launches the Edge Protect Bootloader (EPB) from the RRAM. The EPB authenticates the CM33 secure, CM33 non-secure, and CM55 projects, which are placed in the external flash and loads the TF-M application (M33 secure application) to the SRAM for execution. TF-M creates an isolated space between the M33 secure and M33 non-secure images. TF-M is available in source code format as a library in the *mtb_shared* directory. The CM33 secure application does not have any source files and instead includes this TF-M library from *mtb-shared* for building TF-M firmware.

During the boot sequence, the TF-M's secure partition manager (SPM) creates the SPE and NSPE using TrustZone, MPU, MPC, and PPC protection units. TF-M offers several services that can be used by the non-secure application. These services are placed in independent partitions. The following partitions are initialized by the SPM:

- Internal trusted storage (ITS) 
- Protected storage (PS)
- Crypto
- Initial attestation
- Platform

After initializing the partitions, TF-M launches the M33 NSPE project from external flash, which enables M55 and initialises M33 NSPE <-> M55 NSPE interface using the secure request framework (SRF). The CM33 NS project calls TF-M via the PSA APIs. This code example uses the ITS service demonstration – and it does not use M55, so it is put into Deep Sleep.

For more information on the TF-M library and services, see the **Getting started with Trusted Firmware-M (TF-M) on PSOC&trade; Edge** app note.

A project folder consists of various subfolders, each denoting a specific aspect of the project. The three project folders are as follows:

**Table 1. Application projects**

Project | Description
--------|------------------------
proj_cm33_s | TF-M (SPE)
proj_cm33_ns | M33 NSPE
proj_cm55 | M55 NSPE 

<br>