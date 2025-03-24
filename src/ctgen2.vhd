library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity ctgen2 is
    port (s    : in  state2_t_arr;
          data : in  std_logic_vector(383 downto 0);
          ct   : out std_logic_vector(383 downto 0));
end entity;

architecture parallel of ctgen2 is
    
    signal f0_pt : std_logic_vector(127 downto 0);
    signal f0_ct : std_logic_vector(127 downto 0);
    signal f1_pt : std_logic_vector(127 downto 0);
    signal f1_ct : std_logic_vector(127 downto 0);
    signal f2_pt : std_logic_vector(127 downto 0);
    signal f2_ct : std_logic_vector(127 downto 0);

    signal x0 : std_logic_vector(127 downto 0);
    signal x1 : std_logic_vector(127 downto 0);
    signal x2 : std_logic_vector(127 downto 0);

begin

    x0 <= data(383 downto 256);
    x1 <= data(255 downto 128);
    x2 <= data(127 downto 0);

    f0_pt <= s(0) xor s(1);
    f0_gen : entity f port map (f0_pt, f0_ct);
    ct(383 downto 256) <= f0_ct xor s(3) xor x0;
    
    f1_pt <= s(2) xor s(3);
    f1_gen : entity f port map (f1_pt, f1_ct);
    ct(255 downto 128) <= f1_ct xor s(4) xor x1;
    
    f2_pt <= s(0) xor s(3);
    f2_gen : entity f port map (f2_pt, f2_ct);
    ct(127 downto 0) <= f2_ct xor s(7) xor x2;

end architecture;

