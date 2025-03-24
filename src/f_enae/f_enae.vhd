library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.all;

entity f is
    port (x : in std_logic_vector(0 to 127);
          y : out std_logic_vector(0 to 127));
end entity;

architecture parallel of f is
    
    type enae_t_arr is array (0 to 31) of std_logic_vector(0 to 3);

    signal w, s0, s1, m, pm : enae_t_arr;
    
    type perm_t_arr is array (0 to 31) of integer;
    constant p : perm_t_arr := (
    	0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23,
        8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31
    );

begin

    w_init : for i in 0 to 31 generate
	w(i) <= x(4*i to 4*i+3);
    end generate;

    -- s0
    sbox_gen0 : for i in 0 to 31 generate
        s4_gen0 : entity s4 port map(w(i), s0(i));
    end generate;

    -- m first half
    m(0)  <= s0(0) xor s0(2) xor s0(3) xor s0(5) xor s0(6) xor s0(7) xor s0(8) xor s0(12) xor s0(13);
    m(1)  <= s0(1) xor s0(2) xor s0(3) xor s0(4) xor s0(7) xor s0(8) xor s0(9) xor s0(11) xor s0(12) xor s0(13) xor s0(15);
    m(2)  <= s0(0) xor s0(1) xor s0(3) xor s0(4) xor s0(5) xor s0(6) xor s0(7) xor s0(9);
    m(3)  <= s0(0) xor s0(1) xor s0(2) xor s0(3) xor s0(4) xor s0(6) xor s0(8) xor s0(10) xor s0(11) xor s0(12) xor s0(13) xor s0(14) xor s0(15);
    m(4)  <= s0(1) xor s0(2) xor s0(3) xor s0(5) xor s0(7) xor s0(8) xor s0(9) xor s0(11) xor s0(12) xor s0(15);
    m(5)  <= s0(0) xor s0(2) xor s0(4) xor s0(6) xor s0(7) xor s0(9) xor s0(10) xor s0(11) xor s0(12) xor s0(14);
    m(6)  <= s0(0) xor s0(2) xor s0(3) xor s0(5) xor s0(8) xor s0(12) xor s0(13) xor s0(15);
    m(7)  <= s0(0) xor s0(1) xor s0(2) xor s0(4) xor s0(5) xor s0(9) xor s0(14) xor s0(15);
    m(8)  <= s0(0) xor s0(1) xor s0(3) xor s0(4) xor s0(6) xor s0(8) xor s0(9) xor s0(12) xor s0(13) xor s0(14) xor s0(15);
    m(9)  <= s0(1) xor s0(2) xor s0(4) xor s0(5) xor s0(7) xor s0(8) xor s0(14) xor s0(15);
    m(10) <= s0(3) xor s0(5) xor s0(10) xor s0(11) xor s0(12) xor s0(13) xor s0(14);
    m(11) <= s0(1) xor s0(3) xor s0(4) xor s0(5) xor s0(10) xor s0(11) xor s0(14);
    m(12) <= s0(0) xor s0(1) xor s0(3) xor s0(4) xor s0(5) xor s0(6) xor s0(8) xor s0(10) xor s0(12) xor s0(14) xor s0(15);
    m(13) <= s0(0) xor s0(1) xor s0(3) xor s0(6) xor s0(8) xor s0(10) xor s0(14) xor s0(15);
    m(14) <= s0(3) xor s0(5) xor s0(7) xor s0(8) xor s0(9) xor s0(10) xor s0(11) xor s0(12) xor s0(13) xor s0(14) xor s0(15);
    m(15) <= s0(1) xor s0(3) xor s0(4) xor s0(6) xor s0(7) xor s0(8) xor s0(9) xor s0(12) xor s0(13) xor s0(14);

    -- m second half
    m(16) <= s0(16) xor s0(18) xor s0(19) xor s0(21) xor s0(22) xor s0(23) xor s0(24) xor s0(28) xor s0(29);
    m(17) <= s0(17) xor s0(18) xor s0(19) xor s0(20) xor s0(23) xor s0(24) xor s0(25) xor s0(27) xor s0(28) xor s0(29) xor s0(31);
    m(18) <= s0(16) xor s0(17) xor s0(19) xor s0(20) xor s0(21) xor s0(22) xor s0(23) xor s0(25);
    m(19) <= s0(16) xor s0(17) xor s0(18) xor s0(19) xor s0(20) xor s0(22) xor s0(24) xor s0(26) xor s0(27) xor s0(28) xor s0(29) xor s0(30) xor s0(31);
    m(20) <= s0(17) xor s0(18) xor s0(19) xor s0(21) xor s0(23) xor s0(24) xor s0(25) xor s0(27) xor s0(28) xor s0(31);
    m(21) <= s0(16) xor s0(18) xor s0(20) xor s0(22) xor s0(23) xor s0(25) xor s0(26) xor s0(27) xor s0(28) xor s0(30);
    m(22) <= s0(16) xor s0(18) xor s0(19) xor s0(21) xor s0(24) xor s0(28) xor s0(29) xor s0(31);
    m(23) <= s0(16) xor s0(17) xor s0(18) xor s0(20) xor s0(21) xor s0(25) xor s0(30) xor s0(31);
    m(24) <= s0(16) xor s0(17) xor s0(19) xor s0(20) xor s0(22) xor s0(24) xor s0(25) xor s0(28) xor s0(29) xor s0(30) xor s0(31);
    m(25) <= s0(17) xor s0(18) xor s0(20) xor s0(21) xor s0(23) xor s0(24) xor s0(30) xor s0(31);
    m(26) <= s0(19) xor s0(21) xor s0(26) xor s0(27) xor s0(28) xor s0(29) xor s0(30);
    m(27) <= s0(17) xor s0(19) xor s0(20) xor s0(21) xor s0(26) xor s0(27) xor s0(30);
    m(28) <= s0(16) xor s0(17) xor s0(19) xor s0(20) xor s0(21) xor s0(22) xor s0(24) xor s0(26) xor s0(28) xor s0(30) xor s0(31);
    m(29) <= s0(16) xor s0(17) xor s0(19) xor s0(22) xor s0(24) xor s0(26) xor s0(30) xor s0(31);
    m(30) <= s0(19) xor s0(21) xor s0(23) xor s0(24) xor s0(25) xor s0(26) xor s0(27) xor s0(28) xor s0(29) xor s0(30) xor s0(31);
    m(31) <= s0(17) xor s0(19) xor s0(20) xor s0(22) xor s0(23) xor s0(24) xor s0(25) xor s0(28) xor s0(29) xor s0(30);
    
    -- s1
    sbox_gen1 : for i in 0 to 31 generate
        s4_gen1 : entity s4 port map(m(i), s1(i));
    end generate;
    
    -- perm    
    perm_gen : for i in 0 to 31 generate
        pm(p(i)) <= s1(i);
    end generate;

    y_gen : for i in 0 to 31 generate
	y(4*i to 4*i+3) <= pm(i);
    end generate;
   
end architecture;

