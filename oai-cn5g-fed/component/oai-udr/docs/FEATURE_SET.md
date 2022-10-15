<table style="border-collapse: collapse; border: none;">
  <tr style="border-collapse: collapse; border: none;">
    <td style="border-collapse: collapse; border: none;">
      <a href="http://www.openairinterface.org/">
         <img src="./images/oai_final_logo.png" alt="" border=3 height=50 width=150>
         </img>
      </a>
    </td>
    <td style="border-collapse: collapse; border: none; vertical-align: center;">
      <b><font size = "5">OpenAirInterface AMF Feature Set</font></b>
    </td>
  </tr>
</table>

**Table of Contents**

1. [5GC Service Based Architecture](#1-5gc-service-based-architecture)
2. [OAI UDR Available Interfaces](#2-oai-udr-available-interfaces)
3. [OAI UDR Feature List](#3-oai-udr-feature-list)

# 1. 5GC Service Based Architecture #

![5GC SBA](./images/5gc_sba.png)

# 2. OAI UDR Available Interfaces #

| **ID** | **Interface** | **Status**         | **Comment**               |
| ------ | ------------- | ------------------ | ------------------------- |
| 1      | N35           | :heavy_check_mark: | Communicate with UDM      |
| 2      | N36           | :x:                | Communicate with PCF      |
| 3      | N37           | :x:                | Communicate with NEF      |

# 3. OAI UDR Feature List #

Based on document **3GPP TS 23.501 V16.0.0 §6.2.11**.

| **ID** | **Classification**                                      | **Status**         | **Comments**  |
| ------ | ------------------------------------------------------- | ------------------ |---------------|
| 1      | Storage and retrieval of subscription data by the UDM   | :heavy_check_mark: |               |
| 2      | Storage and retrieval of policy data by the PCF         | :x:                |               |
| 3      | Storage and retrieval of structured data for exposure   | :x:                |               |
| 4      | Application data                                        | :x:                |               |
      