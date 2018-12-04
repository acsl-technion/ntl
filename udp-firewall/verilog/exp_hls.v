// firewall (HLS / SDNet) wrapper for Innova-1 shell

module exp_hls(sbu2mlx_ieee_vendor_id, sbu2mlx_product_id, sbu2mlx_product_version, sbu2mlx_caps, sbu2mlx_caps_len,
   sbu2mlx_caps_addr, mlx2sbu_clk, mlx2sbu_reset, sbu2mlx_leds_on, sbu2mlx_leds_blink, sbu2mlx_watchdog, mlx2sbu_axi4lite_aw_rdy, mlx2sbu_axi4lite_aw_vld, mlx2sbu_axi4lite_aw_addr, mlx2sbu_axi4lite_aw_prot,
   mlx2sbu_axi4lite_w_rdy, mlx2sbu_axi4lite_w_vld, mlx2sbu_axi4lite_w_data, mlx2sbu_axi4lite_w_strobe, mlx2sbu_axi4lite_b_rdy, mlx2sbu_axi4lite_b_vld, mlx2sbu_axi4lite_b_resp, mlx2sbu_axi4lite_ar_rdy, mlx2sbu_axi4lite_ar_vld, mlx2sbu_axi4lite_ar_addr,
   mlx2sbu_axi4lite_ar_prot, mlx2sbu_axi4lite_r_rdy, mlx2sbu_axi4lite_r_vld, mlx2sbu_axi4lite_r_data, mlx2sbu_axi4lite_r_resp, sbu2mlx_axi4mm_aw_rdy, sbu2mlx_axi4mm_aw_vld, sbu2mlx_axi4mm_aw_addr, sbu2mlx_axi4mm_aw_burst, sbu2mlx_axi4mm_aw_cache,
   sbu2mlx_axi4mm_aw_id, sbu2mlx_axi4mm_aw_len, sbu2mlx_axi4mm_aw_lock, sbu2mlx_axi4mm_aw_prot, sbu2mlx_axi4mm_aw_qos, sbu2mlx_axi4mm_aw_region, sbu2mlx_axi4mm_aw_size, sbu2mlx_axi4mm_w_rdy, sbu2mlx_axi4mm_w_vld, sbu2mlx_axi4mm_w_data,
   sbu2mlx_axi4mm_w_last, sbu2mlx_axi4mm_w_strobe, sbu2mlx_axi4mm_b_rdy, sbu2mlx_axi4mm_b_vld, sbu2mlx_axi4mm_b_id, sbu2mlx_axi4mm_b_resp, sbu2mlx_axi4mm_ar_rdy, sbu2mlx_axi4mm_ar_vld, sbu2mlx_axi4mm_ar_addr, sbu2mlx_axi4mm_ar_burst,
   sbu2mlx_axi4mm_ar_cache, sbu2mlx_axi4mm_ar_id, sbu2mlx_axi4mm_ar_len, sbu2mlx_axi4mm_ar_lock, sbu2mlx_axi4mm_ar_prot, sbu2mlx_axi4mm_ar_qos, sbu2mlx_axi4mm_ar_region, sbu2mlx_axi4mm_ar_size, sbu2mlx_axi4mm_r_rdy, sbu2mlx_axi4mm_r_vld,
   sbu2mlx_axi4mm_r_data, sbu2mlx_axi4mm_r_id, sbu2mlx_axi4mm_r_last, sbu2mlx_axi4mm_r_resp, mlx2sbu_axi4stream_rdy, mlx2sbu_axi4stream_vld, mlx2sbu_axi4stream_tdata, mlx2sbu_axi4stream_tkeep, mlx2sbu_axi4stream_tlast, mlx2sbu_axi4stream_tuser,
   mlx2sbu_axi4stream_tid, sbu2mlx_axi4stream_rdy, sbu2mlx_axi4stream_vld, sbu2mlx_axi4stream_tdata, sbu2mlx_axi4stream_tkeep, sbu2mlx_axi4stream_tlast, sbu2mlx_axi4stream_tuser, sbu2mlx_axi4stream_tid, nwp2sbu_axi4stream_rdy, nwp2sbu_axi4stream_vld,
   nwp2sbu_axi4stream_tdata, nwp2sbu_axi4stream_tkeep, nwp2sbu_axi4stream_tlast, nwp2sbu_axi4stream_tuser, nwp2sbu_axi4stream_tid, sbu2nwp_axi4stream_rdy, sbu2nwp_axi4stream_vld, sbu2nwp_axi4stream_tdata, sbu2nwp_axi4stream_tkeep, sbu2nwp_axi4stream_tlast,
   sbu2nwp_axi4stream_tuser, sbu2nwp_axi4stream_tid, cxp2sbu_axi4stream_rdy, cxp2sbu_axi4stream_vld, cxp2sbu_axi4stream_tdata, cxp2sbu_axi4stream_tkeep, cxp2sbu_axi4stream_tlast, cxp2sbu_axi4stream_tuser, cxp2sbu_axi4stream_tid, sbu2cxp_axi4stream_rdy,
   sbu2cxp_axi4stream_vld, sbu2cxp_axi4stream_tdata, sbu2cxp_axi4stream_tkeep, sbu2cxp_axi4stream_tlast, sbu2cxp_axi4stream_tuser, sbu2cxp_axi4stream_tid, nwp2sbu_lossy_has_credits, nwp2sbu_lossless_has_credits, cxp2sbu_lossy_has_credits, cxp2sbu_lossless_has_credits);

output [23:0] sbu2mlx_ieee_vendor_id;
output [15:0] sbu2mlx_product_id;
output [15:0] sbu2mlx_product_version;
output [31:0] sbu2mlx_caps;
output [15:0] sbu2mlx_caps_len;
output [23:0] sbu2mlx_caps_addr;

input         mlx2sbu_clk;// Working clock of 216.25MHz
input         mlx2sbu_reset;// Synchronous reset line to the clock named 'mlx2sbu_clk', active high.
output [7:0]  sbu2mlx_leds_on;
output [7:0]  sbu2mlx_leds_blink;
output        sbu2mlx_watchdog;// Watchdog signal (Should be toggled with freq > 1Hz)

output        mlx2sbu_axi4lite_aw_rdy;
input         mlx2sbu_axi4lite_aw_vld;
input [63:0]  mlx2sbu_axi4lite_aw_addr;
input [2:0]   mlx2sbu_axi4lite_aw_prot;

output        mlx2sbu_axi4lite_w_rdy;
input         mlx2sbu_axi4lite_w_vld;
input [31:0]  mlx2sbu_axi4lite_w_data;
input [3:0]   mlx2sbu_axi4lite_w_strobe;

input         mlx2sbu_axi4lite_b_rdy;
output        mlx2sbu_axi4lite_b_vld;
output [1:0]  mlx2sbu_axi4lite_b_resp;

output        mlx2sbu_axi4lite_ar_rdy;
input         mlx2sbu_axi4lite_ar_vld;
input [63:0]  mlx2sbu_axi4lite_ar_addr;
input [2:0]   mlx2sbu_axi4lite_ar_prot;

input         mlx2sbu_axi4lite_r_rdy;
output        mlx2sbu_axi4lite_r_vld;
output [31:0] mlx2sbu_axi4lite_r_data;
output [1:0]  mlx2sbu_axi4lite_r_resp;

input         sbu2mlx_axi4mm_aw_rdy;
output        sbu2mlx_axi4mm_aw_vld;
output [63:0] sbu2mlx_axi4mm_aw_addr;
output [1:0]  sbu2mlx_axi4mm_aw_burst;
output [3:0]  sbu2mlx_axi4mm_aw_cache;
output [2:0]  sbu2mlx_axi4mm_aw_id;
output [7:0]  sbu2mlx_axi4mm_aw_len;
output        sbu2mlx_axi4mm_aw_lock;
output [2:0]  sbu2mlx_axi4mm_aw_prot;
output [3:0]  sbu2mlx_axi4mm_aw_qos;
output [3:0]  sbu2mlx_axi4mm_aw_region;
output [2:0]  sbu2mlx_axi4mm_aw_size;

input         sbu2mlx_axi4mm_w_rdy;
output        sbu2mlx_axi4mm_w_vld;
output [511:0] sbu2mlx_axi4mm_w_data;
output         sbu2mlx_axi4mm_w_last;
output [63:0]  sbu2mlx_axi4mm_w_strobe;

output         sbu2mlx_axi4mm_b_rdy;
input          sbu2mlx_axi4mm_b_vld;
input [2:0]    sbu2mlx_axi4mm_b_id;
input [1:0]    sbu2mlx_axi4mm_b_resp;

input          sbu2mlx_axi4mm_ar_rdy;
output         sbu2mlx_axi4mm_ar_vld;
output [63:0]  sbu2mlx_axi4mm_ar_addr;
output [1:0]   sbu2mlx_axi4mm_ar_burst;
output [3:0]   sbu2mlx_axi4mm_ar_cache;
output [2:0]   sbu2mlx_axi4mm_ar_id;
output [7:0]   sbu2mlx_axi4mm_ar_len;
output         sbu2mlx_axi4mm_ar_lock;
output [2:0]   sbu2mlx_axi4mm_ar_prot;
output [3:0]   sbu2mlx_axi4mm_ar_qos;
output [3:0]   sbu2mlx_axi4mm_ar_region;
output [2:0]   sbu2mlx_axi4mm_ar_size;

output         sbu2mlx_axi4mm_r_rdy;
input          sbu2mlx_axi4mm_r_vld;
input [511:0]  sbu2mlx_axi4mm_r_data;
input [2:0]    sbu2mlx_axi4mm_r_id;
input          sbu2mlx_axi4mm_r_last;
input [1:0]    sbu2mlx_axi4mm_r_resp;

output         mlx2sbu_axi4stream_rdy;
input          mlx2sbu_axi4stream_vld;
input [255:0]  mlx2sbu_axi4stream_tdata;
input [31:0]   mlx2sbu_axi4stream_tkeep;
input          mlx2sbu_axi4stream_tlast;
input [11:0]   mlx2sbu_axi4stream_tuser;
input [2:0]    mlx2sbu_axi4stream_tid;

input          sbu2mlx_axi4stream_rdy;
output         sbu2mlx_axi4stream_vld;
output [255:0] sbu2mlx_axi4stream_tdata;
output [31:0]  sbu2mlx_axi4stream_tkeep;
output         sbu2mlx_axi4stream_tlast;
output [11:0]  sbu2mlx_axi4stream_tuser;
output [2:0]   sbu2mlx_axi4stream_tid;

output         nwp2sbu_axi4stream_rdy;
input          nwp2sbu_axi4stream_vld;
input [255:0]  nwp2sbu_axi4stream_tdata;
input [31:0]   nwp2sbu_axi4stream_tkeep;
input [0:0]    nwp2sbu_axi4stream_tlast;
input [11:0]   nwp2sbu_axi4stream_tuser;
input [2:0]    nwp2sbu_axi4stream_tid;

input          sbu2nwp_axi4stream_rdy;
output         sbu2nwp_axi4stream_vld;
output [255:0] sbu2nwp_axi4stream_tdata;
output [31:0]  sbu2nwp_axi4stream_tkeep;
output         sbu2nwp_axi4stream_tlast;
output [11:0]  sbu2nwp_axi4stream_tuser;
output [2:0]   sbu2nwp_axi4stream_tid;

output         cxp2sbu_axi4stream_rdy;
input          cxp2sbu_axi4stream_vld;
input [255:0]  cxp2sbu_axi4stream_tdata;
input [31:0]   cxp2sbu_axi4stream_tkeep;
input          cxp2sbu_axi4stream_tlast;
input [11:0]   cxp2sbu_axi4stream_tuser;
input [2:0]    cxp2sbu_axi4stream_tid;

input          sbu2cxp_axi4stream_rdy;
output         sbu2cxp_axi4stream_vld;
output [255:0] sbu2cxp_axi4stream_tdata;
output [31:0]  sbu2cxp_axi4stream_tkeep;
output         sbu2cxp_axi4stream_tlast;
output [11:0]  sbu2cxp_axi4stream_tuser;
output [2:0]   sbu2cxp_axi4stream_tid;

input          nwp2sbu_lossy_has_credits;
input          nwp2sbu_lossless_has_credits;
input          cxp2sbu_lossy_has_credits;
input          cxp2sbu_lossless_has_credits;

///////////////////////////////////////////////////////////////////////////////

// VERSION // ID
assign         sbu2mlx_ieee_vendor_id  = 24'h0002c9;          // Mellanox OUI
assign         sbu2mlx_product_id      = 16'd4;               // HLS Example
assign         sbu2mlx_product_version = 16'd1;
assign         sbu2mlx_caps            = {24'd0, 8'd2};     // Example HLS
assign         sbu2mlx_caps_len        = 16'd0;             // No capablities
assign         sbu2mlx_caps_addr       = 24'd0;             // No capablities

wire           sbu2mlx_axi4mm_aw_user;

wire           temp1;
wire           temp2;

// sbu to cxp data fifo
  wire [255:0] sbu2cxpfifo_axi4stream_tdata;  
  wire         sbu2cxpfifo_axi4stream_vld;
  wire         sbu2cxpfifo_axi4stream_rdy;
  wire [31:0]  sbu2cxpfifo_axi4stream_tkeep;
  wire         sbu2cxpfifo_axi4stream_tlast;
  wire [2:0]   sbu2cxpfifo_axi4stream_tid;
  wire [11:0]  sbu2cxpfifo_axi4stream_tuser;

  wire classify_out_stream_din;
  wire classify_out_stream_write;
  wire classify_out_stream_full_n;
  wire firewall_data_out_tvalid;
  wire firewall_data_out_tready;
  
  wire nwp2sbu_axi4stream_strb = nwp2sbu_axi4stream_vld & nwp2sbu_axi4stream_rdy;

`ifdef BUILD_HLS
  firewall_firewall_top #(
    .C_S_AXI_AXILITES_ADDR_WIDTH(32),
    .C_S_AXI_AXILITES_DATA_WIDTH(32)
  ) firewall0 (

// nica wiring borrowed from example_hls instantiation within ku060_all_exp_hls_wrapper.v:
        .ap_clk(mlx2sbu_clk),
        .ap_rst_n(~mlx2sbu_reset),

        .in_r_TDATA(nwp2sbu_axi4stream_tdata),
        .in_r_TVALID(nwp2sbu_axi4stream_vld),
        .in_r_TREADY(nwp2sbu_axi4stream_rdy),
        .in_r_TKEEP(nwp2sbu_axi4stream_tkeep),
        .in_r_TLAST(nwp2sbu_axi4stream_tlast),

        .data_out_TDATA(sbu2cxpfifo_axi4stream_tdata),
        .data_out_TVALID(firewall_data_out_tvalid),
        .data_out_TREADY(firewall_data_out_tready),
        .data_out_TKEEP(sbu2cxpfifo_axi4stream_tkeep),
        .data_out_TLAST(sbu2cxpfifo_axi4stream_tlast),

        .classify_out_stream_V_V_din(classify_out_stream_din),
        .classify_out_stream_V_V_write(classify_out_stream_write),
        .classify_out_stream_V_V_full_n(classify_out_stream_full_n),

        // axi lite
        .s_axi_AXILiteS_AWREADY(mlx2sbu_axi4lite_aw_rdy),
        .s_axi_AXILiteS_AWVALID(mlx2sbu_axi4lite_aw_vld),
        .s_axi_AXILiteS_AWADDR(mlx2sbu_axi4lite_aw_addr),
        // .axi4lite_aw_prot(mlx2sbu_axi4lite_aw_prot),

        .s_axi_AXILiteS_WVALID(mlx2sbu_axi4lite_w_vld),
        .s_axi_AXILiteS_WREADY(mlx2sbu_axi4lite_w_rdy),
        .s_axi_AXILiteS_WDATA(mlx2sbu_axi4lite_w_data),
        .s_axi_AXILiteS_WSTRB(mlx2sbu_axi4lite_w_strobe),

        .s_axi_AXILiteS_ARVALID(mlx2sbu_axi4lite_ar_vld),
        .s_axi_AXILiteS_ARREADY(mlx2sbu_axi4lite_ar_rdy),
        .s_axi_AXILiteS_ARADDR(mlx2sbu_axi4lite_ar_addr),
        // .axi4lite_ar_prot(mlx2sbu_axi4lite_ar_prot),

        .s_axi_AXILiteS_BVALID(mlx2sbu_axi4lite_b_vld),
        .s_axi_AXILiteS_BREADY(mlx2sbu_axi4lite_b_rdy),
        .s_axi_AXILiteS_BRESP(mlx2sbu_axi4lite_b_resp),

        .s_axi_AXILiteS_RVALID(mlx2sbu_axi4lite_r_vld),
        .s_axi_AXILiteS_RREADY(mlx2sbu_axi4lite_r_rdy),
        .s_axi_AXILiteS_RDATA(mlx2sbu_axi4lite_r_data),
        .s_axi_AXILiteS_RRESP(mlx2sbu_axi4lite_r_resp)
   );

`else
  XilinxSwitch_0 firewall0(
// nica wiring borrowed from example_hls instantiation within ku060_all_exp_hls_wrapper.v:
	.clk_line_rst(mlx2sbu_reset),
	.clk_line(mlx2sbu_clk),
	.clk_lookup_rst(mlx2sbu_reset),
	.clk_lookup(mlx2sbu_clk),
	.clk_control_rst(mlx2sbu_reset),
	.clk_control(mlx2sbu_clk),

        .enable_processing(1'b1),

        .packet_in_packet_in_TDATA(nwp2sbu_axi4stream_tdata),
        .packet_in_packet_in_TVALID(nwp2sbu_axi4stream_vld),
        .packet_in_packet_in_TREADY(nwp2sbu_axi4stream_rdy),
        .packet_in_packet_in_TKEEP(nwp2sbu_axi4stream_tkeep),
        .packet_in_packet_in_TLAST(nwp2sbu_axi4stream_tlast),

        .tuple_in_ctrl_VALID(nwp2sbu_axi4stream_strb),
        .tuple_in_ctrl_DATA(1'b1),

        .packet_out_packet_out_TDATA(sbu2cxpfifo_axi4stream_tdata),
        .packet_out_packet_out_TVALID(firewall_data_out_tvalid),
        .packet_out_packet_out_TREADY(firewall_data_out_tready),
        .packet_out_packet_out_TKEEP(sbu2cxpfifo_axi4stream_tkeep),
        .packet_out_packet_out_TLAST(sbu2cxpfifo_axi4stream_tlast),

        .tuple_out_ctrl_VALID(classify_out_stream_write),
        .tuple_out_ctrl_DATA(classify_out_stream_din),

        // axi lite
        .control_flows_S_AXI_AWREADY(mlx2sbu_axi4lite_aw_rdy),
        .control_flows_S_AXI_AWVALID(mlx2sbu_axi4lite_aw_vld),
        .control_flows_S_AXI_AWADDR(mlx2sbu_axi4lite_aw_addr),
        // .axi4lite_aw_prot(mlx2sbu_axi4lite_aw_prot),

        .control_flows_S_AXI_WVALID(mlx2sbu_axi4lite_w_vld),
        .control_flows_S_AXI_WREADY(mlx2sbu_axi4lite_w_rdy),
        .control_flows_S_AXI_WDATA(mlx2sbu_axi4lite_w_data),
        .control_flows_S_AXI_WSTRB(mlx2sbu_axi4lite_w_strobe),

        .control_flows_S_AXI_ARVALID(mlx2sbu_axi4lite_ar_vld),
        .control_flows_S_AXI_ARREADY(mlx2sbu_axi4lite_ar_rdy),
        .control_flows_S_AXI_ARADDR(mlx2sbu_axi4lite_ar_addr),
        // .axi4lite_ar_prot(mlx2sbu_axi4lite_ar_prot),

        .control_flows_S_AXI_BVALID(mlx2sbu_axi4lite_b_vld),
        .control_flows_S_AXI_BREADY(mlx2sbu_axi4lite_b_rdy),
        .control_flows_S_AXI_BRESP(mlx2sbu_axi4lite_b_resp),

        .control_flows_S_AXI_RVALID(mlx2sbu_axi4lite_r_vld),
        .control_flows_S_AXI_RREADY(mlx2sbu_axi4lite_r_rdy),
        .control_flows_S_AXI_RDATA(mlx2sbu_axi4lite_r_data),
        .control_flows_S_AXI_RRESP(mlx2sbu_axi4lite_r_resp)
   );
`endif

  assign sbu2cxpfifo_axi4stream_tuser = {11'b0, classify_out_stream_din};
  assign sbu2cxpfifo_axi4stream_tid = 3'b0;
  assign sbu2cxpfifo_axi4stream_vld = classify_out_stream_write & firewall_data_out_tvalid;
  wire sbu2cxpfifo_axi4stream_strb = sbu2cxpfifo_axi4stream_vld & sbu2cxpfifo_axi4stream_rdy;
  assign classify_out_stream_full_n = sbu2cxpfifo_axi4stream_strb;
  assign firewall_data_out_tready = sbu2cxpfifo_axi4stream_strb;
  
  // 64 deep store & forward fifos.
  // For more fifo details, see <netperf-verilog_workarea>/sources/xci/axis_data_fifo_0/axis_data_fifo_0.xci
  axis_data_fifo_0 sbu2cxp_data_fifo (
  .s_axis_aresetn(~mlx2sbu_reset),          // input wire s_axis_aresetn
  .s_axis_aclk(mlx2sbu_clk),                // input wire s_axis_aclk
  .s_axis_tvalid(sbu2cxpfifo_axi4stream_vld),            // input wire s_axis_tvalid
  .s_axis_tready(sbu2cxpfifo_axi4stream_rdy),            // output wire s_axis_tready
  .s_axis_tdata(sbu2cxpfifo_axi4stream_tdata),              // input wire [255 : 0] s_axis_tdata
  .s_axis_tkeep(sbu2cxpfifo_axi4stream_tkeep),              // input wire [31 : 0] s_axis_tkeep
  .s_axis_tlast(sbu2cxpfifo_axi4stream_tlast),              // input wire s_axis_tlast
  .s_axis_tid(sbu2cxpfifo_axi4stream_tid),                  // input wire [2 : 0] s_axis_tid
  .s_axis_tuser(sbu2cxpfifo_axi4stream_tuser),              // input wire [11 : 0] s_axis_tuser
  .m_axis_tvalid(sbu2cxp_axi4stream_vld),   // output wire m_axis_tvalid
  .m_axis_tready(sbu2cxp_axi4stream_rdy),   // input wire m_axis_tready
  .m_axis_tdata(sbu2cxp_axi4stream_tdata),  // output wire [255 : 0] m_axis_tdata
  .m_axis_tkeep(sbu2cxp_axi4stream_tkeep),  // output wire [31 : 0] m_axis_tkeep
  .m_axis_tlast(sbu2cxp_axi4stream_tlast),  // output wire m_axis_tlast
  .m_axis_tid(sbu2cxp_axi4stream_tid),      // output wire [2 : 0] m_axis_tid
  .m_axis_tuser(sbu2cxp_axi4stream_tuser)   // output wire [11 : 0] m_axis_tuser
);

assign sbu2nwp_axi4stream_tdata = cxp2sbu_axi4stream_tdata;
assign sbu2nwp_axi4stream_vld = cxp2sbu_axi4stream_vld;
assign cxp2sbu_axi4stream_rdy = sbu2nwp_axi4stream_rdy;
assign sbu2nwp_axi4stream_tkeep = cxp2sbu_axi4stream_tkeep;
assign sbu2nwp_axi4stream_tlast = cxp2sbu_axi4stream_tlast;
assign sbu2nwp_axi4stream_tid = cxp2sbu_axi4stream_tid;
assign sbu2nwp_axi4stream_tuser = cxp2sbu_axi4stream_tuser;

exp_hls_watchdog exp_hls_watchdog(
   .clk(mlx2sbu_clk),
   .reset(mlx2sbu_reset),
   .watchdog_counter(sbu2mlx_watchdog)
);

assign         sbu2mlx_axi4mm_aw_id = 3'd0;
assign         sbu2mlx_axi4mm_ar_id = 3'd0;
assign         sbu2mlx_axi4mm_aw_lock = 1'b0;
assign         sbu2mlx_leds_on =8'd23;
assign         sbu2mlx_leds_blink = 8'd0;

wire           unconnected_sbu2mlx_axi4mm           =
                                                      sbu2mlx_axi4mm_aw_user |
                                                      sbu2mlx_axi4mm_w_rdy |
                                                      sbu2mlx_axi4mm_aw_rdy |
                                                      sbu2mlx_axi4mm_r_vld |
                                                      sbu2mlx_axi4mm_r_last
                                                      ;

wire           unconnect_axi = |{sbu2mlx_axi4mm_ar_rdy,sbu2mlx_axi4mm_r_data,sbu2mlx_axi4mm_r_resp,sbu2mlx_axi4mm_b_vld,sbu2mlx_axi4mm_b_resp,sbu2mlx_axi4mm_b_id,sbu2mlx_axi4mm_r_id};

wire           unconnected_axi_lite = |{mlx2sbu_axi4lite_aw_prot, mlx2sbu_axi4lite_ar_prot};

assign         sbu2mlx_axi4mm_w_data = 512'd0;
assign         sbu2mlx_axi4mm_w_strobe = 64'd0;
assign         sbu2mlx_axi4mm_w_last = 1'b0;
assign         sbu2mlx_axi4mm_w_vld = 1'd0;

assign         sbu2mlx_axi4mm_aw_vld = 1'b0;
assign         sbu2mlx_axi4mm_aw_addr = 64'd0;
assign         sbu2mlx_axi4mm_aw_len = 8'd0;
assign         sbu2mlx_axi4mm_aw_size = 3'd0;
assign         sbu2mlx_axi4mm_aw_burst = 2'd0;
assign         sbu2mlx_axi4mm_aw_cache = 4'd0;
assign         sbu2mlx_axi4mm_aw_prot = 3'd0;
assign         sbu2mlx_axi4mm_aw_qos = 4'd0;
assign         sbu2mlx_axi4mm_aw_region = 4'd0;
assign         sbu2mlx_axi4mm_aw_user = 1'd0;

assign         sbu2mlx_axi4mm_ar_vld    = 1'b0;
assign         sbu2mlx_axi4mm_ar_addr   = 64'd0;
assign         sbu2mlx_axi4mm_ar_len    = 8'd0;
assign         sbu2mlx_axi4mm_ar_size   = 3'd0;
assign         sbu2mlx_axi4mm_ar_burst  = 2'd0;
assign         sbu2mlx_axi4mm_ar_lock   = 1'b0;
assign         sbu2mlx_axi4mm_ar_cache  = 4'd0;
assign         sbu2mlx_axi4mm_ar_prot   = 3'd0;
assign         sbu2mlx_axi4mm_ar_qos    = 4'd0;
assign         sbu2mlx_axi4mm_ar_region = 4'd0;

assign         sbu2mlx_axi4mm_r_rdy = 1'b0;
assign         sbu2mlx_axi4mm_b_rdy = 1'd0;

endmodule
