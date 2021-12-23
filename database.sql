PGDMP     9    
                y            rose_online    12.3    12.3     7           0    0    ENCODING    ENCODING        SET client_encoding = 'UTF8';
                      false            8           0    0 
   STDSTRINGS 
   STDSTRINGS     (   SET standard_conforming_strings = 'on';
                      false            9           0    0 
   SEARCHPATH 
   SEARCHPATH     8   SELECT pg_catalog.set_config('search_path', '', false);
                      false            :           1262    16572    rose_online    DATABASE     �   CREATE DATABASE rose_online WITH TEMPLATE = template0 ENCODING = 'UTF8' LC_COLLATE = 'German_Germany.1252' LC_CTYPE = 'German_Germany.1252';
    DROP DATABASE rose_online;
                postgres    false            �            1259    16573    accounts    TABLE     �   CREATE TABLE public.accounts (
    id integer NOT NULL,
    account_name character varying(32),
    hashed_password character varying(32),
    last_character_online integer
);
    DROP TABLE public.accounts;
       public         heap    postgres    false            �            1259    16576    accounts_id_seq    SEQUENCE     �   CREATE SEQUENCE public.accounts_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 &   DROP SEQUENCE public.accounts_id_seq;
       public          postgres    false    202            ;           0    0    accounts_id_seq    SEQUENCE OWNED BY     C   ALTER SEQUENCE public.accounts_id_seq OWNED BY public.accounts.id;
          public          postgres    false    203            �            1259    16578    items    TABLE     �  CREATE TABLE public.items (
    id integer NOT NULL,
    player_id integer,
    item_type integer,
    item_id integer,
    amount integer DEFAULT 1 NOT NULL,
    slot integer DEFAULT 1 NOT NULL,
    durability integer DEFAULT 35 NOT NULL,
    stat_id integer DEFAULT 0 NOT NULL,
    refinement integer DEFAULT 0 NOT NULL,
    appraised integer DEFAULT 1 NOT NULL,
    socketed integer DEFAULT 0 NOT NULL,
    lifespan integer DEFAULT 1000
);
    DROP TABLE public.items;
       public         heap    postgres    false            �            1259    16589    items_id_seq    SEQUENCE     �   CREATE SEQUENCE public.items_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 #   DROP SEQUENCE public.items_id_seq;
       public          postgres    false    204            <           0    0    items_id_seq    SEQUENCE OWNED BY     =   ALTER SEQUENCE public.items_id_seq OWNED BY public.items.id;
          public          postgres    false    205            �            1259    16591    player_attributes    TABLE     P  CREATE TABLE public.player_attributes (
    player_id integer NOT NULL,
    strength integer DEFAULT 15 NOT NULL,
    dexterity integer DEFAULT 15 NOT NULL,
    intelligence integer DEFAULT 15 NOT NULL,
    concentration integer DEFAULT 15 NOT NULL,
    charm integer DEFAULT 10 NOT NULL,
    sensibility integer DEFAULT 10 NOT NULL
);
 %   DROP TABLE public.player_attributes;
       public         heap    postgres    false            �            1259    16600    players    TABLE     p  CREATE TABLE public.players (
    id integer NOT NULL,
    account_id integer NOT NULL,
    player_name character varying NOT NULL,
    level integer DEFAULT 1 NOT NULL,
    class_id integer DEFAULT 0 NOT NULL,
    sex integer DEFAULT 0 NOT NULL,
    face integer DEFAULT 0 NOT NULL,
    hair integer DEFAULT 0 NOT NULL,
    save_spot_id integer DEFAULT 0 NOT NULL
);
    DROP TABLE public.players;
       public         heap    postgres    false            �            1259    16611    players_id_seq    SEQUENCE     �   CREATE SEQUENCE public.players_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 %   DROP SEQUENCE public.players_id_seq;
       public          postgres    false    207            =           0    0    players_id_seq    SEQUENCE OWNED BY     A   ALTER SEQUENCE public.players_id_seq OWNED BY public.players.id;
          public          postgres    false    208            �
           2604    16613    accounts id    DEFAULT     j   ALTER TABLE ONLY public.accounts ALTER COLUMN id SET DEFAULT nextval('public.accounts_id_seq'::regclass);
 :   ALTER TABLE public.accounts ALTER COLUMN id DROP DEFAULT;
       public          postgres    false    203    202            �
           2604    16614    items id    DEFAULT     d   ALTER TABLE ONLY public.items ALTER COLUMN id SET DEFAULT nextval('public.items_id_seq'::regclass);
 7   ALTER TABLE public.items ALTER COLUMN id DROP DEFAULT;
       public          postgres    false    205    204            �
           2604    16615 
   players id    DEFAULT     h   ALTER TABLE ONLY public.players ALTER COLUMN id SET DEFAULT nextval('public.players_id_seq'::regclass);
 9   ALTER TABLE public.players ALTER COLUMN id DROP DEFAULT;
       public          postgres    false    208    207            .          0    16573    accounts 
   TABLE DATA           \   COPY public.accounts (id, account_name, hashed_password, last_character_online) FROM stdin;
    public          postgres    false    202   "       0          0    16578    items 
   TABLE DATA           �   COPY public.items (id, player_id, item_type, item_id, amount, slot, durability, stat_id, refinement, appraised, socketed, lifespan) FROM stdin;
    public          postgres    false    204   +"       2          0    16591    player_attributes 
   TABLE DATA           |   COPY public.player_attributes (player_id, strength, dexterity, intelligence, concentration, charm, sensibility) FROM stdin;
    public          postgres    false    206   }"       3          0    16600    players 
   TABLE DATA           n   COPY public.players (id, account_id, player_name, level, class_id, sex, face, hair, save_spot_id) FROM stdin;
    public          postgres    false    207   �"       >           0    0    accounts_id_seq    SEQUENCE SET     =   SELECT pg_catalog.setval('public.accounts_id_seq', 7, true);
          public          postgres    false    203            ?           0    0    items_id_seq    SEQUENCE SET     <   SELECT pg_catalog.setval('public.items_id_seq', 411, true);
          public          postgres    false    205            @           0    0    players_id_seq    SEQUENCE SET     <   SELECT pg_catalog.setval('public.players_id_seq', 3, true);
          public          postgres    false    208            �
           2606    16617    accounts accounts_pkey 
   CONSTRAINT     T   ALTER TABLE ONLY public.accounts
    ADD CONSTRAINT accounts_pkey PRIMARY KEY (id);
 @   ALTER TABLE ONLY public.accounts DROP CONSTRAINT accounts_pkey;
       public            postgres    false    202            �
           2606    16619    items items_pkey 
   CONSTRAINT     N   ALTER TABLE ONLY public.items
    ADD CONSTRAINT items_pkey PRIMARY KEY (id);
 :   ALTER TABLE ONLY public.items DROP CONSTRAINT items_pkey;
       public            postgres    false    204            �
           2606    16621 (   player_attributes player_attributes_pkey 
   CONSTRAINT     m   ALTER TABLE ONLY public.player_attributes
    ADD CONSTRAINT player_attributes_pkey PRIMARY KEY (player_id);
 R   ALTER TABLE ONLY public.player_attributes DROP CONSTRAINT player_attributes_pkey;
       public            postgres    false    206            �
           2606    16623    players players_pkey 
   CONSTRAINT     R   ALTER TABLE ONLY public.players
    ADD CONSTRAINT players_pkey PRIMARY KEY (id);
 >   ALTER TABLE ONLY public.players DROP CONSTRAINT players_pkey;
       public            postgres    false    207            �
           2606    16624 2   player_attributes player_attributes_player_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.player_attributes
    ADD CONSTRAINT player_attributes_player_id_fkey FOREIGN KEY (player_id) REFERENCES public.players(id);
 \   ALTER TABLE ONLY public.player_attributes DROP CONSTRAINT player_attributes_player_id_fkey;
       public          postgres    false    2734    207    206            .      x�3�,I-.1���\1z\\\ A?      0   B   x�m��	 1��8LI��]w��sT�{!�Ʌ@��?(��YAtIKՆ��G�?�f��{��n      2      x�3�46�44#NC ����� /q|      3   /   x�3�4�LK,NR@lb�id�i�eNK�4Jp�A$b���� 	2     