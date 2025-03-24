library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity enae2 is
    port (clk   : in std_logic;
          reset : in std_logic;

          nonce : in std_logic_vector(127 downto 0);
          key   : in std_logic_vector(255 downto 0);
       
          last_block : in std_logic;
          
          data      : in std_logic_vector(383 downto 0);
          ad_len    : in std_logic_vector(127 downto 0);
          ad_empty  : in std_logic;
          msg_len   : in std_logic_vector(127 downto 0);
          msg_empty : in std_logic;
          
          ct        : out std_logic_vector(383 downto 0);
          tag       : out std_logic_vector(127 downto 0));
end entity;

architecture structural of enae2 is

    signal k0, k1 : std_logic_vector(127 downto 0);

    signal sr_in   : state2_t_arr;
    signal sr_out  : state2_t_arr;

    signal ru_in      : state2_t_arr;
    signal ru_out     : state2_t_arr; 
    signal ru_aux_in  : std_logic_vector(383 downto 0);
    
    signal sr_load_en, sr56_sel : std_logic;
    signal ru_aux_sel           : std_logic_vector(1 downto 0);

    signal length : std_logic_vector(383 downto 0);
    
    constant z0 : std_logic_vector(127 downto 0) := X"428A2F98D728AE227137449123EF65CD";
    constant z1 : std_logic_vector(127 downto 0) := X"B5C0FBCFEC4D3B2FE9B5DBA58189DBBC";
    constant z2 : std_logic_vector(127 downto 0) := X"7137449123EF65CD428A2F98D728AE22";
    constant  z : std_logic_vector(383 downto 0) := z0 & z1 & z2;

begin

    k0     <= key(255 downto 128);
    k1     <= key(127 downto 0);
    length <= ad_len & msg_len & z0;

    sr_in(0) <= k1               when sr_load_en = '1' else
		ru_out(0) xor k0 when sr56_sel   = '1' else ru_out(0);
    sr_in(1) <= nonce xor k0     when sr_load_en = '1' else
		ru_out(1) xor k1 when sr56_sel   = '1' else ru_out(1); 
    sr_in(2) <= z0               when sr_load_en = '1' else ru_out(2);
    sr_in(3) <= z1               when sr_load_en = '1' else ru_out(3);
    sr_in(4) <= (others => '0')  when sr_load_en = '1' else ru_out(4);
    sr_in(5) <= (others => '0')  when sr_load_en = '1' else ru_out(5);
    sr_in(6) <= (others => '0')  when sr_load_en = '1' else ru_out(6);
    sr_in(7) <= (others => '0')  when sr_load_en = '1' else ru_out(7);
    sr_in(8) <= (others => '0')  when sr_load_en = '1' else ru_out(8);

    s0_reg : entity state_reg port map (clk, sr_in(0), sr_out(0));
    s1_reg : entity state_reg port map (clk, sr_in(1), sr_out(1));
    s2_reg : entity state_reg port map (clk, sr_in(2), sr_out(2));
    s3_reg : entity state_reg port map (clk, sr_in(3), sr_out(3));
    s4_reg : entity state_reg port map (clk, sr_in(4), sr_out(4));
    s5_reg : entity state_reg port map (clk, sr_in(5), sr_out(5));
    s6_reg : entity state_reg port map (clk, sr_in(6), sr_out(6));
    s7_reg : entity state_reg port map (clk, sr_in(7), sr_out(7));
    s8_reg : entity state_reg port map (clk, sr_in(8), sr_out(8));
    
    ru_aux_in <= data   when ru_aux_sel = "01" else
		 length when ru_aux_sel = "10" else z;  
    
    ru : entity roundupdate2 port map (
    	sr_out, ru_out, ru_aux_in(383 downto 256), ru_aux_in(255 downto 128), ru_aux_in(127 downto 0)
    );

    cont : entity controller port map (
        clk, reset, last_block, ad_empty, msg_empty, sr_load_en, sr56_sel, ru_aux_sel
    );

    ctgen  : entity ctgen2  port map(sr_out, data, ct);
    taggen : entity taggen2 port map (sr_out, tag);

end architecture;
         

