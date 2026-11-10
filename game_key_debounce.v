module game_key_debounce (
    input  wire clk,       // 50MHz
    input  wire rst_n,
    input  wire key_in,    // 按下为低
    output reg  key_pulse  // 单周期脉冲，确认按下时输出
);

    localparam CNT_MAX = 20'd300_000 - 1; // 6ms @ 50MHz

    localparam IDLE     = 2'd0,
               DEBOUNCE = 2'd1,
               PRESSED  = 2'd2,
               RELEASE  = 2'd3;

    reg [1:0]  sync;
    reg [1:0]  state;
    reg [19:0] cnt;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) sync <= 2'b11;
        else        sync <= {sync[0], key_in};
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state     <= IDLE;
            cnt       <= 20'd0;
            key_pulse <= 1'b0;
        end else begin
            key_pulse <= 1'b0;  // 默认无脉冲

            case (state)

                IDLE: begin
                    cnt <= 20'd0;
                    if (sync[0] == 1'b0)
                        state <= DEBOUNCE;  // 只启动计数，不输出
                end

                DEBOUNCE: begin
                    if (cnt == CNT_MAX) begin
                        cnt <= 20'd0;
                        if (sync[0] == 1'b0) begin
                            key_pulse <= 1'b1;  // 6ms 稳定低，输出一个脉冲
                            state     <= PRESSED;
                        end else begin
                            state <= IDLE;      // 抖动，不输出
                        end
                    end else begin
                        cnt <= cnt + 1'b1;
                    end
                end

                PRESSED: begin
                    cnt <= 20'd0;
                    if (sync[0] == 1'b1)
                        state <= RELEASE;
                end

                RELEASE: begin
                    if (sync[0] == 1'b0) begin
                        cnt   <= 20'd0;
                        state <= PRESSED;
                    end else if (cnt == CNT_MAX) begin
                        cnt   <= 20'd0;
                        state <= IDLE;
                    end else begin
                        cnt <= cnt + 1'b1;
                    end
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule