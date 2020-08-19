// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("OpenWeatherMap") {
  // clang-format off
  const char* filter_json ="{\"list\":["
      "{\"dt\":true,\"main\":{\"temp\":true},\"weather\":[{\"description\":true}]}"
      "]}";

  const char* input_json = "{\"cod\":\"200\",\"message\":0,\"cnt\":40,\"list\":[{\"dt\":1581498000,\"main\":{\"temp\":3.23,\"feels_like\":-3.63,\"temp_min\":3.23,\"temp_max\":4.62,\"pressure\":1014,\"sea_level\":1014,\"grnd_level\":1010,\"humidity\":58,\"temp_kf\":-1.39},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01d\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":6.19,\"deg\":266},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-12 09:00:00\"},{\"dt\":1581508800,\"main\":{\"temp\":6.09,\"feels_like\":-1.07,\"temp_min\":6.09,\"temp_max\":7.13,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1011,\"humidity\":48,\"temp_kf\":-1.04},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01d\"}],\"clouds\":{\"all\":9},\"wind\":{\"speed\":6.64,\"deg\":268},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-12 12:00:00\"},{\"dt\":1581519600,\"main\":{\"temp\":6.82,\"feels_like\":0.47,\"temp_min\":6.82,\"temp_max\":7.52,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1011,\"humidity\":47,\"temp_kf\":-0.7},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":97},\"wind\":{\"speed\":5.55,\"deg\":267},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-12 15:00:00\"},{\"dt\":1581530400,\"main\":{\"temp\":5.76,\"feels_like\":1.84,\"temp_min\":5.76,\"temp_max\":6.11,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1010,\"humidity\":57,\"temp_kf\":-0.35},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":2.35,\"deg\":232},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-12 18:00:00\"},{\"dt\":1581541200,\"main\":{\"temp\":5.7,\"feels_like\":1.34,\"temp_min\":5.7,\"temp_max\":5.7,\"pressure\":1012,\"sea_level\":1012,\"grnd_level\":1008,\"humidity\":71,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":3.57,\"deg\":198},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-12 21:00:00\"},{\"dt\":1581552000,\"main\":{\"temp\":5.82,\"feels_like\":1.39,\"temp_min\":5.82,\"temp_max\":5.82,\"pressure\":1009,\"sea_level\":1009,\"grnd_level\":1004,\"humidity\":86,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.35,\"deg\":169},\"rain\":{\"3h\":0.5},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-13 00:00:00\"},{\"dt\":1581562800,\"main\":{\"temp\":5.9,\"feels_like\":-0.85,\"temp_min\":5.9,\"temp_max\":5.9,\"pressure\":1000,\"sea_level\":1000,\"grnd_level\":997,\"humidity\":86,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":7.69,\"deg\":178},\"rain\":{\"3h\":1.75},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-13 03:00:00\"},{\"dt\":1581573600,\"main\":{\"temp\":7.52,\"feels_like\":1.74,\"temp_min\":7.52,\"temp_max\":7.52,\"pressure\":993,\"sea_level\":993,\"grnd_level\":988,\"humidity\":88,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":6.84,\"deg\":184},\"rain\":{\"3h\":7.06},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-13 06:00:00\"},{\"dt\":1581584400,\"main\":{\"temp\":7.23,\"feels_like\":0.81,\"temp_min\":7.23,\"temp_max\":7.23,\"pressure\":992,\"sea_level\":992,\"grnd_level\":988,\"humidity\":69,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":49},\"wind\":{\"speed\":6.77,\"deg\":239},\"rain\":{\"3h\":0.25},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-13 09:00:00\"},{\"dt\":1581595200,\"main\":{\"temp\":7.67,\"feels_like\":2.81,\"temp_min\":7.67,\"temp_max\":7.67,\"pressure\":991,\"sea_level\":991,\"grnd_level\":987,\"humidity\":75,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":73},\"wind\":{\"speed\":4.93,\"deg\":235},\"rain\":{\"3h\":0.75},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-13 12:00:00\"},{\"dt\":1581606000,\"main\":{\"temp\":8.83,\"feels_like\":3.23,\"temp_min\":8.83,\"temp_max\":8.83,\"pressure\":993,\"sea_level\":993,\"grnd_level\":990,\"humidity\":64,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":83},\"wind\":{\"speed\":5.7,\"deg\":293},\"rain\":{\"3h\":0.38},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-13 15:00:00\"},{\"dt\":1581616800,\"main\":{\"temp\":7.42,\"feels_like\":1.77,\"temp_min\":7.42,\"temp_max\":7.42,\"pressure\":1000,\"sea_level\":1000,\"grnd_level\":996,\"humidity\":71,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":54},\"wind\":{\"speed\":5.81,\"deg\":307},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-13 18:00:00\"},{\"dt\":1581627600,\"main\":{\"temp\":5.82,\"feels_like\":0.89,\"temp_min\":5.82,\"temp_max\":5.82,\"pressure\":1007,\"sea_level\":1007,\"grnd_level\":1003,\"humidity\":79,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01n\"}],\"clouds\":{\"all\":6},\"wind\":{\"speed\":4.76,\"deg\":300},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-13 21:00:00\"},{\"dt\":1581638400,\"main\":{\"temp\":5.58,\"feels_like\":2.09,\"temp_min\":5.58,\"temp_max\":5.58,\"pressure\":1011,\"sea_level\":1011,\"grnd_level\":1007,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03n\"}],\"clouds\":{\"all\":47},\"wind\":{\"speed\":2.73,\"deg\":326},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-14 00:00:00\"},{\"dt\":1581649200,\"main\":{\"temp\":4.27,\"feels_like\":1.72,\"temp_min\":4.27,\"temp_max\":4.27,\"pressure\":1014,\"sea_level\":1014,\"grnd_level\":1010,\"humidity\":85,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":69},\"wind\":{\"speed\":1.24,\"deg\":295},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-14 03:00:00\"},{\"dt\":1581660000,\"main\":{\"temp\":3.91,\"feels_like\":1.54,\"temp_min\":3.91,\"temp_max\":3.91,\"pressure\":1016,\"sea_level\":1016,\"grnd_level\":1012,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":85},\"wind\":{\"speed\":0.98,\"deg\":211},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-14 06:00:00\"},{\"dt\":1581670800,\"main\":{\"temp\":4.77,\"feels_like\":0.74,\"temp_min\":4.77,\"temp_max\":4.77,\"pressure\":1017,\"sea_level\":1017,\"grnd_level\":1013,\"humidity\":78,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":3.19,\"deg\":184},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-14 09:00:00\"},{\"dt\":1581681600,\"main\":{\"temp\":9.03,\"feels_like\":4,\"temp_min\":9.03,\"temp_max\":9.03,\"pressure\":1016,\"sea_level\":1016,\"grnd_level\":1012,\"humidity\":73,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":5.43,\"deg\":206},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-14 12:00:00\"},{\"dt\":1581692400,\"main\":{\"temp\":9.86,\"feels_like\":4.22,\"temp_min\":9.86,\"temp_max\":9.86,\"pressure\":1014,\"sea_level\":1014,\"grnd_level\":1010,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":6.58,\"deg\":209},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-14 15:00:00\"},{\"dt\":1581703200,\"main\":{\"temp\":9.48,\"feels_like\":4.8,\"temp_min\":9.48,\"temp_max\":9.48,\"pressure\":1013,\"sea_level\":1013,\"grnd_level\":1009,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":5.6,\"deg\":206},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-14 18:00:00\"},{\"dt\":1581714000,\"main\":{\"temp\":10.03,\"feels_like\":6.48,\"temp_min\":10.03,\"temp_max\":10.03,\"pressure\":1013,\"sea_level\":1013,\"grnd_level\":1009,\"humidity\":93,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.75,\"deg\":226},\"rain\":{\"3h\":3.13},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-14 21:00:00\"},{\"dt\":1581724800,\"main\":{\"temp\":9.48,\"feels_like\":6.25,\"temp_min\":9.48,\"temp_max\":9.48,\"pressure\":1013,\"sea_level\":1013,\"grnd_level\":1009,\"humidity\":89,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":3.87,\"deg\":214},\"rain\":{\"3h\":2.38},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-15 00:00:00\"},{\"dt\":1581735600,\"main\":{\"temp\":9.12,\"feels_like\":7.08,\"temp_min\":9.12,\"temp_max\":9.12,\"pressure\":1011,\"sea_level\":1011,\"grnd_level\":1007,\"humidity\":96,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":2.43,\"deg\":194},\"rain\":{\"3h\":1},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-15 03:00:00\"},{\"dt\":1581746400,\"main\":{\"temp\":10.32,\"feels_like\":6.71,\"temp_min\":10.32,\"temp_max\":10.32,\"pressure\":1009,\"sea_level\":1009,\"grnd_level\":1004,\"humidity\":95,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":5.05,\"deg\":196},\"rain\":{\"3h\":1.75},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-15 06:00:00\"},{\"dt\":1581757200,\"main\":{\"temp\":11.57,\"feels_like\":5.85,\"temp_min\":11.57,\"temp_max\":11.57,\"pressure\":1006,\"sea_level\":1006,\"grnd_level\":1002,\"humidity\":85,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":7.91,\"deg\":205},\"rain\":{\"3h\":1.44},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-15 09:00:00\"},{\"dt\":1581768000,\"main\":{\"temp\":12.25,\"feels_like\":4.46,\"temp_min\":12.25,\"temp_max\":12.25,\"pressure\":1003,\"sea_level\":1003,\"grnd_level\":998,\"humidity\":78,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":10.65,\"deg\":201},\"rain\":{\"3h\":1.81},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-15 12:00:00\"},{\"dt\":1581778800,\"main\":{\"temp\":12.19,\"feels_like\":3.17,\"temp_min\":12.19,\"temp_max\":12.19,\"pressure\":998,\"sea_level\":998,\"grnd_level\":994,\"humidity\":80,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":12.52,\"deg\":204},\"rain\":{\"3h\":3.5},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-15 15:00:00\"},{\"dt\":1581789600,\"main\":{\"temp\":12.25,\"feels_like\":4.15,\"temp_min\":12.25,\"temp_max\":12.25,\"pressure\":996,\"sea_level\":996,\"grnd_level\":992,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":11.42,\"deg\":215},\"rain\":{\"3h\":4.88},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-15 18:00:00\"},{\"dt\":1581800400,\"main\":{\"temp\":12.64,\"feels_like\":5.85,\"temp_min\":12.64,\"temp_max\":12.64,\"pressure\":994,\"sea_level\":994,\"grnd_level\":990,\"humidity\":76,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":9.22,\"deg\":217},\"rain\":{\"3h\":6.88},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-15 21:00:00\"},{\"dt\":1581811200,\"main\":{\"temp\":12.96,\"feels_like\":4.03,\"temp_min\":12.96,\"temp_max\":12.96,\"pressure\":988,\"sea_level\":988,\"grnd_level\":984,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":12.88,\"deg\":211},\"rain\":{\"3h\":5.63},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-16 00:00:00\"},{\"dt\":1581822000,\"main\":{\"temp\":13.13,\"feels_like\":5.17,\"temp_min\":13.13,\"temp_max\":13.13,\"pressure\":987,\"sea_level\":987,\"grnd_level\":982,\"humidity\":82,\"temp_kf\":0},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":11.49,\"deg\":246},\"rain\":{\"3h\":7.25},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-16 03:00:00\"},{\"dt\":1581832800,\"main\":{\"temp\":9.07,\"feels_like\":0.79,\"temp_min\":9.07,\"temp_max\":9.07,\"pressure\":990,\"sea_level\":990,\"grnd_level\":986,\"humidity\":75,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":10.18,\"deg\":255},\"rain\":{\"3h\":2},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-16 06:00:00\"},{\"dt\":1581843600,\"main\":{\"temp\":8.05,\"feels_like\":-0.9,\"temp_min\":8.05,\"temp_max\":8.05,\"pressure\":994,\"sea_level\":994,\"grnd_level\":990,\"humidity\":51,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":9.65,\"deg\":245},\"rain\":{\"3h\":1.19},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-16 09:00:00\"},{\"dt\":1581854400,\"main\":{\"temp\":9.54,\"feels_like\":0.13,\"temp_min\":9.54,\"temp_max\":9.54,\"pressure\":996,\"sea_level\":996,\"grnd_level\":991,\"humidity\":41,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":94},\"wind\":{\"speed\":10.03,\"deg\":243},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-16 12:00:00\"},{\"dt\":1581865200,\"main\":{\"temp\":9.08,\"feels_like\":-0.35,\"temp_min\":9.08,\"temp_max\":9.08,\"pressure\":996,\"sea_level\":996,\"grnd_level\":991,\"humidity\":44,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":89},\"wind\":{\"speed\":10.15,\"deg\":246},\"rain\":{\"3h\":0.25},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-16 15:00:00\"},{\"dt\":1581876000,\"main\":{\"temp\":7.41,\"feels_like\":-1.34,\"temp_min\":7.41,\"temp_max\":7.41,\"pressure\":996,\"sea_level\":996,\"grnd_level\":992,\"humidity\":50,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":94},\"wind\":{\"speed\":9.21,\"deg\":240},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-16 18:00:00\"},{\"dt\":1581886800,\"main\":{\"temp\":6.42,\"feels_like\":-1.7,\"temp_min\":6.42,\"temp_max\":6.42,\"pressure\":997,\"sea_level\":997,\"grnd_level\":993,\"humidity\":58,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":67},\"wind\":{\"speed\":8.52,\"deg\":236},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-16 21:00:00\"},{\"dt\":1581897600,\"main\":{\"temp\":6.03,\"feels_like\":-2.65,\"temp_min\":6.03,\"temp_max\":6.03,\"pressure\":996,\"sea_level\":996,\"grnd_level\":993,\"humidity\":51,\"temp_kf\":0},\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03n\"}],\"clouds\":{\"all\":38},\"wind\":{\"speed\":8.94,\"deg\":240},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-17 00:00:00\"},{\"dt\":1581908400,\"main\":{\"temp\":5.62,\"feels_like\":-2.86,\"temp_min\":5.62,\"temp_max\":5.62,\"pressure\":995,\"sea_level\":995,\"grnd_level\":991,\"humidity\":53,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01n\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":8.67,\"deg\":241},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-17 03:00:00\"},{\"dt\":1581919200,\"main\":{\"temp\":5.51,\"feels_like\":-2.41,\"temp_min\":5.51,\"temp_max\":5.51,\"pressure\":995,\"sea_level\":995,\"grnd_level\":991,\"humidity\":61,\"temp_kf\":0},\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03n\"}],\"clouds\":{\"all\":35},\"wind\":{\"speed\":8.2,\"deg\":244},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2020-02-17 06:00:00\"}],\"city\":{\"id\":2643743,\"name\":\"London\",\"coord\":{\"lat\":51.5085,\"lon\":-0.1257},\"country\":\"GB\",\"population\":1000000,\"timezone\":0,\"sunrise\":1581492085,\"sunset\":1581527294}}";

  const char* expected_json = "{\"list\":["
      "{\"dt\":1581498000,\"main\":{\"temp\":3.23},\"weather\":[{\"description\":\"clear sky\"}]},"
      "{\"dt\":1581508800,\"main\":{\"temp\":6.09},\"weather\":[{\"description\":\"clear sky\"}]},"
      "{\"dt\":1581519600,\"main\":{\"temp\":6.82},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581530400,\"main\":{\"temp\":5.76},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581541200,\"main\":{\"temp\":5.7},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581552000,\"main\":{\"temp\":5.82},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581562800,\"main\":{\"temp\":5.9},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581573600,\"main\":{\"temp\":7.52},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581584400,\"main\":{\"temp\":7.23},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581595200,\"main\":{\"temp\":7.67},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581606000,\"main\":{\"temp\":8.83},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581616800,\"main\":{\"temp\":7.42},\"weather\":[{\"description\":\"broken clouds\"}]},"
      "{\"dt\":1581627600,\"main\":{\"temp\":5.82},\"weather\":[{\"description\":\"clear sky\"}]},"
      "{\"dt\":1581638400,\"main\":{\"temp\":5.58},\"weather\":[{\"description\":\"scattered clouds\"}]},"
      "{\"dt\":1581649200,\"main\":{\"temp\":4.27},\"weather\":[{\"description\":\"broken clouds\"}]},"
      "{\"dt\":1581660000,\"main\":{\"temp\":3.91},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581670800,\"main\":{\"temp\":4.77},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581681600,\"main\":{\"temp\":9.03},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581692400,\"main\":{\"temp\":9.86},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581703200,\"main\":{\"temp\":9.48},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581714000,\"main\":{\"temp\":10.03},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581724800,\"main\":{\"temp\":9.48},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581735600,\"main\":{\"temp\":9.12},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581746400,\"main\":{\"temp\":10.32},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581757200,\"main\":{\"temp\":11.57},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581768000,\"main\":{\"temp\":12.25},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581778800,\"main\":{\"temp\":12.19},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581789600,\"main\":{\"temp\":12.25},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581800400,\"main\":{\"temp\":12.64},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581811200,\"main\":{\"temp\":12.96},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581822000,\"main\":{\"temp\":13.13},\"weather\":[{\"description\":\"moderate rain\"}]},"
      "{\"dt\":1581832800,\"main\":{\"temp\":9.07},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581843600,\"main\":{\"temp\":8.05},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581854400,\"main\":{\"temp\":9.54},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581865200,\"main\":{\"temp\":9.08},\"weather\":[{\"description\":\"light rain\"}]},"
      "{\"dt\":1581876000,\"main\":{\"temp\":7.41},\"weather\":[{\"description\":\"overcast clouds\"}]},"
      "{\"dt\":1581886800,\"main\":{\"temp\":6.42},\"weather\":[{\"description\":\"broken clouds\"}]},"
      "{\"dt\":1581897600,\"main\":{\"temp\":6.03},\"weather\":[{\"description\":\"scattered clouds\"}]},"
      "{\"dt\":1581908400,\"main\":{\"temp\":5.62},\"weather\":[{\"description\":\"clear sky\"}]},"
      "{\"dt\":1581919200,\"main\":{\"temp\":5.51},\"weather\":[{\"description\":\"scattered clouds\"}]}"
      "]}";
  // clang-format on

  StaticJsonDocument<512> filter;

  REQUIRE(deserializeJson(filter, filter_json) == DeserializationError::Ok);

  DynamicJsonDocument doc(16384);

  REQUIRE(
      deserializeJson(doc, input_json, DeserializationOption::Filter(filter)) ==
      DeserializationError::Ok);

  REQUIRE(doc.as<std::string>() == expected_json);
}
