const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CssMinimizerPlugin = require("css-minimizer-webpack-plugin");

const path = require("path");

module.exports = {
  entry: "./script.js",
  output: {
    path: path.resolve(__dirname, "dist"),
    filename: "main.js",
  },
  module: {
    rules: [
      {
        test: /\.s[ac]ss$/i,
        use: [
          MiniCssExtractPlugin.loader,
          "css-loader",
          "sass-loader",
        ],
      },
      {
        test: /\.(png|svg|jpg|jpeg|gif)$/i,
        type: 'asset/resource',
      },
    ]
    },
    optimization: {
        minimizer: [
          new CssMinimizerPlugin(),
        ],
      },

  plugins: [
    new HtmlWebpackPlugin({
    inject: true,
    hash: false,
    filename: 'index.html',
    template: path.resolve(__dirname, 'src', 'index.html')
  }),
    new MiniCssExtractPlugin({
        filename: 'css/[name].css',
      }),
    ]
};
