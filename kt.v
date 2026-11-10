module kt(
    input  wire clk,
    input  wire key_in,
    output reg [7:0] led
);

    wire key_pulse;

    game_key_debounce k1(
        .clk(clk),
        .rst_n(1'd1),
        .key_in(key_in),
        .key_pulse(key_pulse)
    );

    initial led = 8'b1111_1111;

    always @(posedge clk) begin
        if (key_pulse)
            led <= led - 1'b1;
    end

endmodule