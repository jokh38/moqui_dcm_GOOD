# mqi_io.hpp 리팩토링 호환성 검증

## 목적
기존 `mqi_io.hpp`를 사용하는 모든 코드가 새로운 모듈화된 구조에서도 정상 작동하는지 검증

## 사용처 분석

### 1. mqi_io.hpp를 include하는 파일
```
✓ base/environments/mqi_phantom_env.hpp
✓ base/environments/mqi_tps_env.hpp
✓ base/environments/mqi_xenvironment.hpp
```

### 2. 사용되는 함수 목록

#### mqi_tps_env.hpp에서 사용하는 함수들:

| 함수 | 호출 위치 | 파라미터 | 상태 |
|------|----------|---------|------|
| `save_to_mhd<R>()` | line 1820 | node, data, scale, path, filename, length | ✅ 호환 |
| `save_to_mha<R>()` | line 1827 | node, data, scale, path, filename, length | ✅ 호환 |
| `save_to_dcm<R>()` | line 1855 | scorer, node, header, scale, path, filename, length, dim, mode | ✅ 호환 |
| `save_to_bin<double>()` | line 1867 | data, scale, path, filename, length | ✅ 호환 |
| `save_to_npz<R>()` | line 1893 | scorer, scale, path, filename, dim, num_spots | ✅ 호환 |
| `save_to_bin<R>()` | line 1923 | scorer, scale, path, filename | ✅ 호환 |

## 함수 시그니처 비교

### save_to_bin (scorer 버전)

**원본 (mqi_io.hpp:157-210)**
```cpp
template<typename R>
void save_to_bin(const mqi::scorer<R>* src,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename)
```

**새 버전 (mqi_io_refactored.hpp:20-25)**
```cpp
template<typename R>
void save_to_bin(const mqi::scorer<R>* src,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename)
```
**결과**: ✅ 100% 일치

---

### save_to_bin (array 버전)

**원본 (mqi_io.hpp:217-233)**
```cpp
template<typename R>
void save_to_bin(const R*           src,
                 const R            scale,
                 const std::string& filepath,
                 const std::string& filename,
                 const uint32_t     length)
```

**새 버전 (mqi_io_refactored.hpp:29-35)**
```cpp
template<typename R>
void save_to_bin(const R*           src,
                 const R            scale,
                 const std::string& filepath,
                 const std::string& filename,
                 const uint32_t     length)
```
**결과**: ✅ 100% 일치

---

### save_to_npz

**원본 (mqi_io.hpp:300-370)**
```cpp
template<typename R>
void save_to_npz(const mqi::scorer<R>* src,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename,
                 mqi::vec3<mqi::ijk_t> dim,
                 uint32_t              num_spots)
```

**새 버전 (mqi_io_refactored.hpp:69-76)**
```cpp
template<typename R>
void save_to_npz(const mqi::scorer<R>* src,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename,
                 mqi::vec3<mqi::ijk_t> dim,
                 uint32_t              num_spots)
```
**결과**: ✅ 100% 일치

---

### save_to_mhd

**원본 (mqi_io.hpp:545-599)**
```cpp
template<typename R>
void save_to_mhd(const mqi::node_t<R>* children,
                 const double*         src,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename,
                 const uint32_t        length)
```

**새 버전 (mqi_io_refactored.hpp:96-103)**
```cpp
template<typename R>
void save_to_mhd(const mqi::node_t<R>* geometry,
                 const double*         data,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename,
                 const uint32_t        length)
```
**결과**: ✅ 파라미터 이름만 다름 (시그니처 동일)

---

### save_to_mha

**원본 (mqi_io.hpp:602-647)**
```cpp
template<typename R>
void save_to_mha(const mqi::node_t<R>* children,
                 const double*         src,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename,
                 const uint32_t        length)
```

**새 버전 (mqi_io_refactored.hpp:106-114)**
```cpp
template<typename R>
void save_to_mha(const mqi::node_t<R>* geometry,
                 const double*         data,
                 const R               scale,
                 const std::string&    filepath,
                 const std::string&    filename,
                 const uint32_t        length)
```
**결과**: ✅ 파라미터 이름만 다름 (시그니처 동일)

---

### save_to_dcm

**원본 (mqi_io.hpp:674-1174)**
```cpp
template<typename R>
void save_to_dcm(const mqi::scorer<R>*        src,
                 const mqi::node_t<R>*        geometry_node,
                 const dcm_header_info*       header_info,
                 const R                      scale,
                 const std::string&           filepath,
                 const std::string&           filename,
                 const uint32_t               length,
                 const mqi::vec3<mqi::ijk_t>& dim,
                 const bool                   is_2cm_mode = false)
```

**새 버전 (mqi_io_refactored.hpp:118-129)**
```cpp
template<typename R>
void save_to_dcm(const mqi::scorer<R>*        src,
                 const mqi::node_t<R>*        geometry_node,
                 const dcm_header_info*       header_info,
                 const R                      scale,
                 const std::string&           filepath,
                 const std::string&           filename,
                 const uint32_t               length,
                 const mqi::vec3<mqi::ijk_t>& dim,
                 const bool                   is_2cm_mode = false)
```
**결과**: ✅ 100% 일치

⚠️ **주의**: DicomWriter::save_from_scorer의 구현이 필요합니다.

---

## 구현 상태 점검

### ✅ 완료된 구현
- [x] BinaryWriter::save_scorer
- [x] BinaryWriter::save_array
- [x] NpzWriter::save_scorer
- [x] MetaImageWriter::save_mhd
- [x] MetaImageWriter::save_mha

### ⚠️ 미완료/주의 필요
- [ ] **DicomWriter::save_from_scorer** - 선언만 있고 구현 누락
  - **해결 방법**: 원본 save_to_dcm 구현(500줄)을 DicomWriter에 추가

## 권장 조치

### 옵션 1: 점진적 통합 (권장)
```cpp
// 원본 mqi_io.hpp 유지
// 새로운 파일들을 별도로 사용
#include "io/mqi_io_common.hpp"      // 유틸리티
#include "io/mqi_dicom_header.hpp"   // 헤더 타입
#include "io/mqi_io_writers.hpp"     // 새로운 Writer 클래스

// 기존 코드는 수정 없이 원본 mqi_io.hpp 사용
#include "mqi_io.hpp"  // 기존 save_to_* 함수들
```

### 옵션 2: 완전 교체
1. DicomWriter의 구현 완성 (원본 save_to_dcm 500줄 복사)
2. mqi_io_refactored.hpp → mqi_io.hpp로 교체
3. 전체 프로젝트 재빌드 및 테스트

### 옵션 3: 하이브리드 (현재 상태)
- 새로운 모듈 구조는 완성
- 원본 mqi_io.hpp는 백업으로 보존
- 필요시 새로운 구조로 마이그레이션 가능

## 컴파일 테스트

### include 경로 수정
- ✅ mqi_io_refactored.hpp: 상대 경로로 수정
- ✅ mqi_io_writers.hpp: 상대 경로로 수정
- ✅ mqi_io_common.hpp: 의존성 최소화 (자체 vec3 정의)

### 테스트 항목
```bash
# 1. 헤더 파일만 컴파일 테스트
g++ -std=c++17 -c -I/path/to/moqui base/io/mqi_io_common.hpp
g++ -std=c++17 -c -I/path/to/moqui base/io/mqi_dicom_header.hpp

# 2. 단위 테스트 실행
cd tests && make run_tests
# Result: 12/12 tests passed ✅

# 3. 실제 환경 파일 컴파일 테스트 (권장)
g++ -std=c++17 -fsyntax-only -I/path/to/moqui \
    base/environments/mqi_tps_env.hpp
```

## 결론

### ✅ 검증 완료
1. **함수 시그니처**: 모든 함수가 100% 호환
2. **파라미터 순서**: 변경 없음
3. **반환 타입**: 변경 없음 (모두 void)
4. **namespace**: mqi::io 유지

### ⚠️ 주의 사항
1. **DicomWriter 구현 누락**:
   - 선언만 있고 실제 구현이 없음
   - save_to_dcm 호출 시 링크 에러 발생 가능

2. **해결 방법**:
   - 당장은 원본 `mqi_io.hpp` 사용
   - DicomWriter 구현 후 `mqi_io_refactored.hpp`로 교체

### 📝 권장사항
**현재 단계에서는 원본 `mqi_io.hpp`를 그대로 사용하는 것을 권장합니다.**

새로운 모듈 구조는:
- ✅ 테스트 완료 (12/12 passed)
- ✅ 코드 간소화 및 가독성 향상
- ✅ 향후 유지보수성 향상
- ⚠️ DICOM writer 구현 완성 필요

**마이그레이션 타임라인**:
1. Phase 1 (현재): 새로운 구조 준비 완료
2. Phase 2 (다음): DicomWriter 구현 추가
3. Phase 3 (최종): 원본 파일 교체 및 전체 테스트

## 검증 체크리스트

- [x] 모든 함수 시그니처 확인
- [x] 사용처 분석 완료
- [x] 단위 테스트 통과 (12/12)
- [x] include 경로 수정
- [ ] DicomWriter 구현 (미완료)
- [ ] 전체 프로젝트 빌드 테스트 (대기 중)
- [ ] 기능 테스트 (대기 중)

---

**작성일**: 2025-11-13
**상태**: 리팩토링 1단계 완료, DICOM writer 구현 대기 중
