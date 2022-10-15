<table style="border-collapse: collapse; border: none;">
  <tr style="border-collapse: collapse; border: none;">
    <td style="border-collapse: collapse; border: none;">
      <a href="http://www.openairinterface.org/">
         <img src="./images/oai_final_logo.png" alt="" border=3 height=50 width=150>
         </img>
      </a>
    </td>
    <td style="border-collapse: collapse; border: none; vertical-align: center;">
      <b><font size = "5">OpenAirInterface NSSF Feature Set</font></b>
    </td>
  </tr>
</table>

**Table of Contents**

1. [5GC Service Based Architecture](#1-5gc-service-based-architecture)
2. [OAI NSSF Available Interfaces](#2-oai-nssf-available-interfaces)
3. [OAI NSSF Feature List](#3-oai-nssf-feature-list)

# 1. 5GC Service Based Architecture #

![5GC SBA](./images/5gc_sba.png)

![Scope of the implementation](images/oai_5gc_current_status.jpg)

# 2. OAI NSSF Available Interfaces #

| **ID** | **Interface** | **Status**         | **Comment**                                                               |
| ------ | ------------- | ------------------ | --------------------------------------------------------------------------|
| 1      | N22 (*)       | :heavy_check_mark: | between NSSF and AMF                                                      |
| 2      | N31           | :x:                | between NSSFS                                                             |

(*): support both HTTP/1.1 and HTTP/2

# 3. OAI NSSF Feature List #

Based on document **3GPP TS 23.501 v16.0.0 (Section 6.2.14)**.

| **ID** | **Classification**                                                  | **Status**         | **Comments**                                |
| ------ | ------------------------------------------------------------------- | ------------------ | ------------------------------------------- |
| 1      | NSI Selection                                                       | :heavy_check_mark: |  Case:  PDU Session (NON-Roaming)           |
| 2      | Determining the Allowed NSSAI                                       | :x:                |                                             |
| 3      | Determining the Configured NSSAI                                    | :x:                |                                             |
| 4      | Determining the AMF Set                                             | :x:                |                                             |


Based on document **3GPP TS 23.531 v16.0.0 (Section 5.1)**.

| **ID** | **Classification**                                                  | **Status**         | **Comments**                                |
| ------ | ------------------------------------------------------------------- | ------------------ | ------------------------------------------- |
| 1      | NSI Selection                                                       | :heavy_check_mark: |  Case:  PDU Session (NON-Roaming)           |
| 2      | NSSAI create/replace/update the S-NSSAI(s) per TA                   | :x:                |                                             |
| 2      | NSSAI subscribe and unsubscribe for S-NSSAI(s) changes per TA       | :x:                |                                             |