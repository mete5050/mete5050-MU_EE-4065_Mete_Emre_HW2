Mehmet Mete EKER 150722013
Muhammet Emre MEMİLİ-150721033

This project performs basic grayscale image processing on an STM32 microcontroller using a test image stored as a header file. All processing is done in C without external libraries, demonstrating how fundamental DSP techniques can run directly on embedded hardware.

First, the MCU computes a 256-level histogram of the original image and sends the results as ASCII text over UART. This allows the histogram to be inspected inside STM32CubeIDE and provides a statistical view of the image.

Next, histogram equalization is implemented by calculating the cumulative distribution function (CDF) and building a lookup table to remap pixel values. This enhances image contrast. Both the equalized image (sent in PGM format) and its updated histogram are transmitted over UART.

The project then applies spatial filtering using 3×3 convolution kernels. A low-pass filter produces a blurred output, while a high-pass filter highlights edges. Each filtered image is sent to the PC for viewing.

Finally, a 3×3 median filter is used to reduce noise while preserving edges. The median-filtered result is also transferred over UART, completing the image-processing pipeline.


<img width="64" height="64" alt="HighPass" src="https://github.com/user-attachments/assets/dabcdf0a-7546-4d2b-9b47-77fe6d3b225e" />

<img width="64" height="64" alt="Histogram" src="https://github.com/user-attachments/assets/502de390-9339-4bb7-9a30-f368c3c306ef" />

<img width="64" height="64" alt="LowPass" src="https://github.com/user-attachments/assets/d7072c28-177d-4619-9995-974714f4c3ca" />

<img width="64" height="64" alt="Medain" src="https://github.com/user-attachments/assets/2489aa42-64d6-42f4-9f37-4dd154a99ed5" />

| Original Histogram | Equalized Histogram |
| ------------------ | ------------------- |
| H[010] = 12        | H[000] = 19         |
| H[019] = 7         | H[001] = 11         |
| H[021] = 11        | H[002] = 19         |
| H[025] = 12        | H[003] = 19         |
| H[029] = 7         | H[004] = 16         |
| H[033] = 19        | H[005] = 9          |
| H[034] = 10        | H[006] = 20         |
| H[035] = 6         | H[007] = 10         |
| H[036] = 2         | H[008] = 10         |
| H[038] = 7         | H[009] = 24         |
| H[039] = 12        | H[011] = 31         |
| H[042] = 8         | H[012] = 16         |
| H[043] = 10        | H[013] = 15         |
| H[045] = 10        | H[014] = 17         |
| H[046] = 19        | H[016] = 37         |
| H[048] = 5         | H[017] = 10         |
| H[051] = 31        | H[018] = 12         |
| H[056] = 16        | H[019] = 23         |
| H[057] = 15        | H[020] = 19         |
| H[058] = 17        | H[021] = 17         |
| H[061] = 29        | H[023] = 19         |
| H[063] = 8         | H[024] = 26         |
| H[064] = 10        | H[026] = 27         |
| H[067] = 12        | H[027] = 23         |
| H[068] = 23        | H[028] = 12         |
| H[071] = 19        | H[029] = 18         |
| H[072] = 17        | H[030] = 11         |
| H[074] = 19        | H[031] = 18         |
| H[076] = 26        | H[032] = 21         |
| H[077] = 27        | H[034] = 24         |
| H[081] = 23        | H[035] = 16         |
| H[082] = 12        | H[036] = 25         |
| H[084] = 18        | H[037] = 17         |
| H[085] = 11        | H[038] = 16         |
| H[087] = 18        | H[040] = 27         |
| H[088] = 21        | H[041] = 7          |
| H[090] = 24        | H[042] = 28         |
| H[092] = 16        | H[043] = 18         |
| H[093] = 17        | H[044] = 17         |
| H[094] = 8         | H[046] = 30         |
| H[095] = 7         | H[048] = 26         |
| H[096] = 10        | H[050] = 37         |
| H[097] = 7         | H[051] = 16         |
| H[098] = 9         | H[052] = 18         |
| H[099] = 21        | H[053] = 13         |
| H[100] = 6         | H[054] = 18         |
| H[101] = 7         | H[055] = 9          |
| H[103] = 16        | H[056] = 24         |
| H[104] = 12        | H[057] = 16         |
| H[105] = 5         | H[058] = 14         |
| H[106] = 13        | H[059] = 12         |
| H[107] = 11        | H[060] = 19         |
| H[108] = 6         | H[061] = 13         |
| H[110] = 17        | H[062] = 16         |
| H[111] = 13        | H[063] = 20         |
| H[113] = 21        | H[064] = 20         |
| H[117] = 5         | H[065] = 14         |
| H[118] = 28        | H[066] = 13         |
| H[120] = 3         | H[067] = 11         |
| H[121] = 6         | H[068] = 22         |
| H[123] = 9         | H[069] = 16         |
| H[126] = 7         | H[070] = 14         |
| H[127] = 7         | H[072] = 35         |
| H[128] = 4         | H[073] = 19         |
| H[130] = 7         | H[074] = 13         |
| H[133] = 8         | H[075] = 19         |
| H[134] = 5         | H[076] = 15         |
| H[135] = 5         | H[077] = 13         |
| H[138] = 4         | H[078] = 15         |
| H[139] = 5         | H[079] = 21         |
| H[140] = 4         | H[081] = 27         |
| H[141] = 9         | H[082] = 12         |
| H[142] = 10        | H[083] = 17         |
| H[147] = 14        | H[084] = 19         |
| H[150] = 3         | H[085] = 14         |
| H[153] = 4         | H[086] = 18         |
| H[154] = 9         | H[088] = 32         |
| H[156] = 4         | H[089] = 8          |
| H[157] = 5         | H[091] = 31         |
| H[159] = 5         | H[092] = 18         |
| H[162] = 12        | H[093] = 17         |
| H[166] = 12        | H[097] = 60         |
| H[168] = 7         | H[098] = 20         |
| H[171] = 10        | H[103] = 86         |
| H[173] = 3         | H[111] = 128        |
| H[174] = 10        | H[122] = 169        |
| H[178] = 2         | H[255] = 2135       |
| H[179] = 4         | —                   |
| H[180] = 13        | —                   |
| H[181] = 7         | —                   |
| H[183] = 14        | —                   |
| H[185] = 6         | —                   |
| H[187] = 14        | —                   |
| H[188] = 9         | —                   |
| H[189] = 4         | —                   |
| H[190] = 11        | —                   |
| H[191] = 15        | —                   |
| H[192] = 7         | —                   |
| H[193] = 16        | —                   |
| H[195] = 10        | —                   |
| H[196] = 4         | —                   |
| H[197] = 35        | —                   |
| H[198] = 10        | —                   |
| H[200] = 9         | —                   |
| H[201] = 4         | —                   |
| H[202] = 4         | —                   |
| H[203] = 5         | —                   |
| H[204] = 13        | —                   |
| H[205] = 6         | —                   |
| H[206] = 10        | —                   |
| H[208] = 5         | —                   |
| H[209] = 13        | —                   |
| H[212] = 9         | —                   |
| H[216] = 6         | —                   |
| H[217] = 6         | —                   |
| H[218] = 5         | —                   |
| H[222] = 4         | —                   |
| H[223] = 6         | —                   |
| H[228] = 27        | —                   |
| H[232] = 12        | —                   |
| H[235] = 17        | —                   |
| H[237] = 19        | —                   |
| H[239] = 8         | —                   |
| H[240] = 6         | —                   |
| H[241] = 18        | —                   |
| H[243] = 32        | —                   |
| H[245] = 8         | —                   |
| H[247] = 31        | —                   |
| H[248] = 18        | —                   |
| H[249] = 17        | —                   |
| H[250] = 60        | —                   |
| H[251] = 20        | —                   |
| H[252] = 86        | —                   |
| H[253] = 128       | —                   |
| H[254] = 169       | —                   |
| H[255] = 2135      | —                   |
