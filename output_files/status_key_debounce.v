module status_key_debounce(
    input  wire clk,      // 50MHz
    input  wire rst_n,    // 低有效复位
    input  wire key_in,   // 按键输入，按下为低
    output reg  key_out   // 消抖输出，低有效
);

    // 20ms @ 50MHz
    localparam CNT_MAX = 20'd1_000_000 - 1;

    localparam IDLE    = 2'd0,
               DEBOUNCE= 2'd1,
               PRESSED = 2'd2,
               RELEASE = 2'd3;

    reg [1:0]  sync;
    reg [1:0]  state;
    reg [19:0] cnt;

    // 两级同步
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            sync <= 2'b11;
        else
            sync <= {sync[0], key_in};
    end

    // 状态机 + 消抖计数
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state   <= IDLE;
            cnt     <= 20'd0;
            key_out <= 1'b1;
        end else begin
            case (state)

                IDLE: begin
                    cnt     <= 20'd0;
                    key_out <= 1'b1;

                    // 检测到按下，立即响应，不等消抖完成
                    if (sync[0] == 1'b0) begin
                        key_out <= 1'b0;
                        state   <= DEBOUNCE;
                    end
                end

                DEBOUNCE: begin
                    if (sync[0] == 1'b1) begin
                        // 是抖动，恢复释放状态
                        key_out <= 1'b1;
                        cnt     <= 20'd0;
                        state   <= IDLE;
                    end else if (cnt == CNT_MAX) begin
                        // 20ms 稳定按下，确认
                        key_out <= 1'b0;
                        cnt     <= 20'd0;
                        state   <= PRESSED;
                    end else begin
                        cnt <= cnt + 1'b1;
                    end
                end

                PRESSED: begin
                    key_out <= 1'b0;
                    cnt     <= 20'd0;

                    // 检测释放，进入释放消抖
                    if (sync[0] == 1'b1)
                        state <= RELEASE;
                end

                RELEASE: begin
                    if (sync[0] == 1'b0) begin
                        // 释放抖动，保持按下
                        key_out <= 1'b0;
                        cnt     <= 20'd0;
                        state   <= PRESSED;
                    end else if (cnt == CNT_MAX) begin
                        // 释放稳定
                        key_out <= 1'b1;
                        cnt     <= 20'd0;
                        state   <= IDLE;
                    end else begin
                        cnt <= cnt + 1'b1;
                    end
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule