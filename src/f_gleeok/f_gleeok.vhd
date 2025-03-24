library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.all;

entity f is
    port (x   : in std_logic_vector(0 to 127);
          y   : out std_logic_vector(0 to 127));
end entity;

architecture parallel of f is

    signal sbox_out0, sbox_out1, sbox_out2, theta_out0, theta_out1, pi_out0, pi_out1 : std_logic_vector(0 to 127);

begin

    -- s0
    sbox_gen0 : for i in 0 to 15 generate
        s3_gen0 : entity s3 port map(x(i*8 to i*8+2), sbox_out0(i*8 to i*8+2));
        s5_gen0 : entity s5 port map(x(i*8+3 to i*8+7), sbox_out0(i*8+3 to i*8+7));
    end generate;

    theta_gen0 : for i in 0 to 127 generate
        theta_out0(i) <= sbox_out0((i+12) mod 128) xor sbox_out0((i+31) mod 128) xor sbox_out0((i+86) mod 128);
    end generate;
        
    pi_gen0 : for i in 0 to 127 generate
        pi_out0(i) <= theta_out0((i*117) mod 128);
    end generate;
   
    -- 1 
    sbox_gen1 : for i in 0 to 15 generate
        s3_gen1 : entity s3 port map(pi_out0(i*8 to i*8+2), sbox_out1(i*8 to i*8+2));
        s5_gen1 : entity s5 port map(pi_out0(i*8+3 to i*8+7), sbox_out1(i*8+3 to i*8+7));
    end generate;

    theta_gen1 : for i in 0 to 127 generate
        theta_out1(i) <= sbox_out1((i+12) mod 128) xor sbox_out1((i+31) mod 128) xor sbox_out1((i+86) mod 128);
    end generate;
        
    pi_gen1 : for i in 0 to 127 generate
        pi_out1(i) <= theta_out1((i*117) mod 128);
    end generate;
    
    -- 2 
    sbox_gen2 : for i in 0 to 15 generate
        s3_gen2 : entity s3 port map(pi_out1(i*8 to i*8+2), sbox_out2(i*8 to i*8+2));
        s5_gen2 : entity s5 port map(pi_out1(i*8+3 to i*8+7), sbox_out2(i*8+3 to i*8+7));
    end generate;

    y <= sbox_out2;
   
end architecture;

