library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity ctgen1 is
    port (s    : in  state1_t_arr;
          data : in  std_logic_vector(255 downto 0);
          ct   : out std_logic_vector(255 downto 0));
end entity;

architecture parallel of ctgen1 is
    
    signal f0_pt : std_logic_vector(127 downto 0);
    signal f0_ct : std_logic_vector(127 downto 0);
    signal f1_pt : std_logic_vector(127 downto 0);
    signal f1_ct : std_logic_vector(127 downto 0);

    signal x0 : std_logic_vector(127 downto 0);
    signal x1 : std_logic_vector(127 downto 0);

begin

    x0 <= data(255 downto 128);
    x1 <= data(127 downto 0);

    f0_pt <= s(0) xor s(1);
    f0_gen : entity f port map (f0_pt, f0_ct);
    ct(255 downto 128) <= f0_ct xor s(3) xor x0;
    
    f1_pt <= s(2) xor s(3);
    f1_gen : entity f port map (f1_pt, f1_ct);
    ct(127 downto 0) <= f1_ct xor s(4) xor x1;

end architecture;

