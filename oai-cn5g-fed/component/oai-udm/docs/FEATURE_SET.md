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
2. [OAI UDM Available Interfaces](#2-oai-udm-available-interfaces)
3. [OAI UDM Feature List](#3-oai-udm-feature-list)

# 1. 5GC Service Based Architecture #

![5GC SBA](./images/5gc_sba.png)

# 2. OAI UDM Available Interfaces #

| **ID** | **Interface** | **Status**         | **Comment**               |
| ------ | ------------- | ------------------ | ------------------------- |
| 1      | N8            | :heavy_check_mark: | Communicate with AMF      |
| 2      | N10           | :heavy_check_mark: | Communicate with SMF      |
| 3      | N13           | :heavy_check_mark: | Communicate with AUSF     |
| 4      | N35           | :heavy_check_mark: | Communicate with UDR      |

# 3. OAI UDM Feature List #

Based on document **3GPP TS 23.501 V16.0.0 §6.2.7**.

| **ID** | **Classification**                                      | **Status**         | **Comments**  |
| ------ | ------------------------------------------------------- | ------------------ |---------------|
| 1      | Generation of 3GPP AKA Authentication Credentials       | :heavy_check_mark: |               |
| 2      | User Identification Handling                            | :heavy_check_mark: |               |
| 3      | Support of de-concealment of SUCI                       | :heavy_check_mark: |               |
| 4      | Access authorization based on subscription data         | :x:                |               |
| 5      | UE's Serving NF Registration Management                 | :x:                |               |
| 6      | Support to service/session continuity                   | :x:                |               |
| 7      | MT-SMS delivery support                                 | :x:                |               |
| 8      | Lawful Intercept Functionality                          | :x:                |               |
| 9      | Subscription management                                 | :x:                |               |
| 10     | SMS management                                          | :x:                |               |
| 11     | 5GLAN group management handling                         | :x:                |               |
| 12     | Support of external parameter provisioning              | :x:                |               |      