const config = {
    content: [
        "./src/**/*.{html,js,svelte,ts}",
        "./node_modules/flowbite-svelte/**/*.{html,js,svelte,ts}",
    ],

    plugins: [
        require('flowbite/plugin')
    ],

    darkMode: 'class',

    theme: {
        extend: {
            colors: {
                primary: {
                    50: '#c598ff',
                    100: '#b17eff',
                    200: '#8e4dff',
                    300: '#6c1cff',
                    400: '#5a00ff',
                    500: '#4c00e6',
                    600: '#3e00b3',
                    700: '#300080',
                    800: '#22004d',
                    900: '#14001a',
                    950: '#0d0003'
                },
                secondary: {
                    50: '#f5f5f5',
                    100: '#ebebeb',
                    200: '#d4d4d4',
                    300: '#bdbdbd',
                    400: '#8f8f8f',
                    500: '#616161',
                    600: '#575757',
                    700: '#4d4d4d',
                    800: '#434343',
                    900: '#353535',
                    950: '#2c2c2c'
                },
            }
        }
    }
};

module.exports = config;